package com.robotad.waterserver.data;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.annotations.Expose;
import org.hibernate.annotations.Type;
import org.hibernate.annotations.TypeDef;

import javax.persistence.*;
import java.sql.Timestamp;
import java.time.Instant;

@Entity
@Table(name = "records")
@TypeDef(name = "JsonbType", typeClass = JsonbType.class)
public class Record {
    @Expose
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long _id;

    @Expose
    @Type(type = "JsonbType")
    private JsonObject data;

    @Expose
    private Timestamp timestamp;

    public Record() {}

    public Record(JsonObject data) {
        this.data = data;
        this.timestamp = Timestamp.from(Instant.now());
    }

    public Long get_id() {
        return _id;
    }

    public void setData(String jsonString) {
        this.data = new JsonParser().parse(jsonString).getAsJsonObject();
    }

    public JsonObject getData() {
        return this.data;
    }

    public Timestamp getTimestamp() {
        return this.timestamp;
    }

    @Override
    public String toString () {
        Gson gson = new GsonBuilder().excludeFieldsWithoutExposeAnnotation().create();
        return gson.toJson(this);
    }
}

