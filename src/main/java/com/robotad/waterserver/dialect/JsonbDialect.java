package com.robotad.waterserver.dialect;

import org.hibernate.dialect.PostgreSQL95Dialect;

import java.sql.Types;

public class JsonbDialect extends PostgreSQL95Dialect {
    public JsonbDialect(){
        this.registerColumnType(Types.JAVA_OBJECT, "jsonb");
    }
}
