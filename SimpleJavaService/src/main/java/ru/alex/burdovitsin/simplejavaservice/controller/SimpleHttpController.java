package ru.alex.burdovitsin.simplejavaservice.controller;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RestController;

import java.util.Random;

@RestController
public class SimpleHttpController {

    @GetMapping("/echo")
    public String echo() {
        return "Hello World!";
    }

    @GetMapping("/param/{param}")
    public String param(@PathVariable String param) {
        return "Param = " + param;
    }
}
