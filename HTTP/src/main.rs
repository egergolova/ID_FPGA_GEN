use actix_web::{http::Method, web, App, HttpResponse, HttpServer, Responder};
use core::time;
use std::str::Bytes;
use std::thread::sleep;
use std::{fs, sync::Mutex};
use std::io::prelude::*;
use std::time::Duration;
use serialport::*;
use std::io::{self, Write, Read};
use serde::{Deserialize, Serialize};
use once_cell::sync::Lazy;

#[derive(Serialize, Deserialize)]
struct ResponseCipher {
    key: Vec<u8>,
    text: Vec<u8>,
}

static mut _CURR_FOLDER: String = String::new();
static PORT: Lazy<Mutex<Box<dyn SerialPort>>> = Lazy::new(|| {
    let port_name = "COM21";
    let baud_rate = 9600;

    let port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(4000))
        .open()
        .expect("Failed to open port");
    println!("Гатова! {}", port_name);
    Mutex::new(port)
});

async fn cipher_handler(data: web::Json<String>) -> impl Responder {
    let mut buffer: Vec<u8> = vec![0; 10024]; 
    let mut port = PORT.lock().unwrap();

    let command = format!("CIPHER{}", char::from(1));
    let _ = port.write(command.as_bytes());
    let _ = port.write((format!("{}{}", data.clone(), char::from(2))).as_bytes());
    println!("Sent command: {}", command);
    let mut text_FPGA: Vec<u8>= Vec::new();

    match port.read(&mut buffer) {
        Ok(n) => {
            text_FPGA =  buffer.iter().cloned().filter(|&c| c != 0).collect();
            println!("Received text from FPGA: {:?}", text_FPGA.clone());
        },
        Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => {
            println!("Timeout reached without data");
        },
        Err(e) => println!("Smth with data"),
    };

    let (part1, part2) = buffer.split_at(buffer.iter().position(|&x| x == 46).expect("No dot between key and text"));

    let response = ResponseCipher {
        key: part1.to_vec(),
        text: part2[1..].to_vec().iter().cloned().filter(|&c| c != 0).collect(),
    };

    HttpResponse::Ok()
        .content_type("application/json")
        .json(response)
}

async fn decipher_handler(data: web::Json<String>) -> impl Responder {
    let mut buffer: Vec<u8> = vec![0; 10024]; 
    let mut port = PORT.lock().unwrap();

    let command = format!("DECIPHER{}", char::from(1));
    let _ = port.write(command.as_bytes());
    
    let parts: Vec<&str> = data.split(';').collect();
    let data_key = parts[0].to_string() + ".";
    let mut data_text: Vec<u8> = parts[1].split(',')
    .map(|part| part.parse().expect("Cannot parse the")).collect();
    data_text.push(2u8);

    let _ = port.write(data_key.as_bytes());
    let _ = port.write(&data_text);

    println!("Sent command: {}", command);
    println!("Sent: {}", data.clone());
    let mut text_FPGA: Vec<u8>= Vec::new();
    let mut text_2_FPGA: String = String::new();
    match port.read(&mut buffer) {
        Ok(n) => {
            text_FPGA =  buffer.iter().cloned().filter(|&c| c != 0).collect();
            text_2_FPGA = String::from_utf8_lossy(&buffer)
                                                        .chars()
                                                        .filter(|&c| c != '\0')
                                                        .collect::<String>();

            println!("Received text from FPGA: {:?}", text_FPGA.clone());
            println!("Received text from FPGA: {:?}", text_2_FPGA.clone());
        },
        Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => {
            println!("Timeout reached without data");
        },
        Err(e) => println!("Smth with data"),
    };


    HttpResponse::Ok().body(text_FPGA.clone())
}

async fn rand_handler() -> impl Responder {
    let mut port = PORT.lock().unwrap();
    let command = format!("GET_RAND{}", char::from(1));
    let _ = port.write(command.as_bytes());
    let mut text = String::new();

    let mut buffer: Vec<u8> = vec![0; 1024]; 
    match port.read(&mut buffer) {
        Ok(n) => {
            text = String::from_utf8_lossy(&buffer)
                                                        .chars()
                                                        .filter(|&c| c != '\0')
                                                        .collect::<String>();
                                                    println!("Received text: {:?}", text);
        },
        Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => {
            println!("Timeout reached without data");
        },
        Err(e) => println!("Smth with data"),
    };

    println!("RAND!");
    HttpResponse::Ok().body(text.clone())
}

async fn id_handler() -> impl Responder {
    let mut port = PORT.lock().unwrap();
    let command = format!("GET_ID{}", char::from(1));
    let _ = port.write(command.as_bytes());
    let mut text = String::new();

    let mut buffer: Vec<u8> = vec![0; 1024]; 
    match port.read(&mut buffer) {
        Ok(n) => {
            text = String::from_utf8_lossy(&buffer)
                                                        .chars()
                                                        .filter(|&c| c != '\0')
                                                        .collect::<String>();
        },
        Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => {
            println!("Timeout reached without data");
        },
        Err(e) => println!("Smth with data"),
    };
    println!("ID!");
    HttpResponse::Ok().body(text.clone())
}

#[actix_web::main]
async fn main() -> std::io::Result<()>{
    println!("Слухаю!");
    HttpServer::new(||
        App::new()
            .route("/", web::route().method(Method::from_bytes(b"CIPHER").unwrap()).to(cipher_handler))
            .route("/", web::route().method(Method::from_bytes(b"DECIPHER").unwrap()).to(decipher_handler))
            .route("/", web::route().method(Method::from_bytes(b"RAND").unwrap()).to(rand_handler))
            .route("/", web::route().method(Method::from_bytes(b"ID").unwrap()).to(id_handler))
    )
    .bind(("192.168.175.201", 402))?
    .run()
    .await
}