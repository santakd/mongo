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

#include "mongo/db/matcher/schema/expression_internal_schema_num_properties.h"

namespace mongo {

void InternalSchemaNumPropertiesMatchExpression::debugString(StringBuilder& debug,
                                                             int indentationLevel) const {
    _debugAddSpace(debug, indentationLevel);
    BSONObjBuilder builder;
    serialize(&builder, true);
    debug << builder.obj().toString() << "\n";
}

void InternalSchemaNumPropertiesMatchExpression::serialize(BSONObjBuilder* out,
                                                           bool includePath) const {
    out->append(_name, _numProperties);
}

bool InternalSchemaNumPropertiesMatchExpression::equivalent(const MatchExpression* other) const {
    if (matchType() != other->matchType())
        return false;
    const InternalSchemaNumPropertiesMatchExpression* otherMaxProperties =
        static_cast<const InternalSchemaNumPropertiesMatchExpression*>(other);
    return _numProperties == otherMaxProperties->_numProperties;
}
}  // namespace mongo
