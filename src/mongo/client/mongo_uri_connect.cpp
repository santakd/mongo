/**
 *    Copyright (C) 2018-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/platform/basic.h"

#include "mongo/client/mongo_uri.h"

#include "mongo/client/authenticate.h"
#include "mongo/client/dbclient_base.h"
#include "mongo/util/str.h"

namespace mongo {

DBClientBase* MongoURI::connect(StringData applicationName,
                                std::string& errmsg,
                                boost::optional<double> socketTimeoutSecs,
                                const ClientAPIVersionParameters* apiParameters) const {
    OptionsMap::const_iterator it = _options.find("socketTimeoutMS");
    if (it != _options.end() && !socketTimeoutSecs) {
        try {
            socketTimeoutSecs = std::stod(it->second) / 1000;
        } catch (const std::exception& e) {
            uasserted(ErrorCodes::BadValue,
                      str::stream() << "Unable to parse socketTimeoutMS value" << causedBy(e));
        }
    }

    auto swConn = _connectString.connect(
        applicationName, socketTimeoutSecs.value_or(0.0), this, apiParameters);
    if (!swConn.isOK()) {
        errmsg = swConn.getStatus().reason();
        return nullptr;
    }

    if (!getSetName().empty()) {
        // When performing initial topology discovery, don't bother authenticating
        // since we will be immediately restarting our connect loop to a single node.
        return swConn.getValue().release();
    }

    auto connection = std::move(swConn.getValue());
    if (!connection->authenticatedDuringConnect()) {
        auto optAuthObj = makeAuthObjFromOptions(connection->getMaxWireVersion(),
                                                 connection->getIsPrimarySaslMechanisms());
        if (optAuthObj) {
            connection->auth(optAuthObj.get());
        }
    }

    return connection.release();
}

}  // namespace mongo
