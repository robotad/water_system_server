package com.robotad.waterserver.controller;

import com.robotad.waterserver.data.Record;
import com.robotad.waterserver.service.WaterService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class Controller {
    private WaterService waterService;

    @Autowired
    public Controller(WaterService waterService) {
        this.waterService = waterService;
    }

    @RequestMapping(method = RequestMethod.GET,
            produces = MediaType.TEXT_PLAIN_VALUE)
    public ResponseEntity get(){
        String displayStr = waterService.getDisplay();
        return ResponseEntity.ok(displayStr);
    }

    @RequestMapping(method = RequestMethod.POST,
            consumes = MediaType.APPLICATION_JSON_VALUE,
            produces = MediaType.APPLICATION_JSON_VALUE)
    public ResponseEntity receive(@RequestBody String data){
        Record record = waterService.createRecord(data);
        return ResponseEntity.ok(record);
    }
}
