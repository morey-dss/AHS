#include <SPI.h>
#include <LoRa.h>

float rssiOffset=0;
float emaAlpha=0.25;
float emaValue=0;
float lastRssi=0;
float rssiBuffer[7];
int rssiIndex=0;

float calibrateRssi(float ölçülen,float reference){
    return reference-ölçülen;
}

float applyCalibration(float rssi){
    return rssi+rssiOffset;
}

float emaFilter(float rssi){
    emaValue=emaValue+(emaAlpha*(rssi-emaValue));
    return emaValue;
}

float medyan7(float r){
    rssiBuffer[rssiIndex]=r;
    rssiIndex=(rssiIndex+1)%7;
    float arr[7];
    for(int i=0;i<7;i++) arr[i]=rssiBuffer[i];
    for(int i=0;i<6;i++){
        for(int j=0;j<6-i;j++){
            if(arr[j]>arr[j+1]){
                float t=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=t;
            }
        }
    }
    return arr[3];
}

float stabilityGate(float current,float previous,float threshold){
    float diff=current-previous;
    if(diff<0) diff=-diff;
    if(diff>threshold) return previous;
    return current;
}

float smoothingLayer(float raw){
    float c=applyCalibration(raw);
    float m=medyan7(c);
    float e=emaFilter(m);
    float s=stabilityGate(e,lastRssi,4.0);
    lastRssi=s;
    return s;
}

float estimateDistance(float rssi){
    float pathLoss=abs(rssi);
    float d=pow(10,(pathLoss-40)/28.0);
    return d;
}

float normalize(float x,float a,float b){
    if(x<a) x=a;
    if(x>b) x=b;
    return (x-a)/(b-a);
}

float pulseRate(float dist){
    if(dist<1) dist=1;
    if(dist>40) dist=40;
    float n=normalize(40-dist,0,39);
    float rate=100+(n*900);
    return rate;
}

void beep(int duration){
    digitalWrite(3,HIGH);
    delay(duration);
    digitalWrite(3,LOW);
}

void setup(){
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
    LoRa.begin(433E6);
}

void loop(){
    int packetSize=LoRa.parsePacket();
    if(packetSize){
        int rawRssi=LoRa.packetRssi();
        float filt=smoothingLayer(rawRssi);
        float dist=estimateDistance(filt);
        float rate=pulseRate(dist);
        int interval=(int)rate;
        digitalWrite(4,HIGH);
        beep(30);
        digitalWrite(4,LOW);
        delay(interval);
    }
}

