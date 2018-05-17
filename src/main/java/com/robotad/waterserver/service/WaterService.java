package com.robotad.waterserver.service;

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.robotad.waterserver.data.Record;
import com.robotad.waterserver.data.RecordRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;

import java.awt.print.Pageable;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

@org.springframework.stereotype.Service
public class WaterService {
    private static final int DISPLAY_LENGTH = 10;
    private RecordRepository recordRepository;

    @Autowired
    public WaterService(RecordRepository recordRepository) {
        this.recordRepository = recordRepository;
    }

    public Record createRecord(String sourceString) {
        JsonElement sourceElement = new JsonParser().parse(sourceString);
        if (sourceElement.isJsonObject()){
            JsonObject source = sourceElement.getAsJsonObject();
            JsonObject target = new JsonObject();
            addPropertyIfExists(source, target, "timestamp");
            addPropertyIfExists(source, target, "state");
            addPropertyIfExists(source, target, "moisture1");
            addPropertyIfExists(source, target, "water_flow");
            addPropertyIfExists(source, target, "water_fail_reps");
            addPropertyIfExists(source, target, "water_fail_sets");
            Record record = new Record(target);
            return recordRepository.save(record);
        } else {
            return null;
        }
    }

    public String getDisplay() {
        org.springframework.data.domain.Pageable pageable =
                PageRequest.of(0, DISPLAY_LENGTH, Sort.by(Sort.Direction.DESC, "_id"));
        Page<Record> records = recordRepository.findAll(pageable);

        StringBuilder displayMoisture1 = new StringBuilder();
        for (Record record : records) {
            int moisture1 = record.getData().get("moisture1").getAsNumber().intValue();
            String state = record.getData().get("state").getAsString();
            String displayStr = state.equals("WATERING")? "#" : "-";

            displayMoisture1
                    .append(IntStream.range(0, moisture1).mapToObj(i -> displayStr).collect(Collectors.joining("")))
                    .append("\n");
        }
        return displayMoisture1.toString();
    }

    private void addPropertyIfExists(JsonObject source, JsonObject target, String property){
        if (source.has(property)){
            if (source.get(property).isJsonPrimitive()) {
                target.add(property, source.getAsJsonPrimitive(property));
            }
        }
    }
}
