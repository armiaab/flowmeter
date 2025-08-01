#include <SPI.h>
#include <LoRa.h>

#define ss 5
#define rst 14
#define dio0 2
#define FLOW_METER 4

volatile int pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
float flowRate = 0.0;
float totalVolume = 0.0;
unsigned long previousTime = 0;
const float calibrationFactor = 7.5;
const unsigned long MIN_PULSE_INTERVAL = 10;
const unsigned long FLOW_TIMEOUT = 3000;

const int MAX_SAMPLES = 200; // 10 / 0.05 = 200
float flowRateSamples[MAX_SAMPLES];
int sampleIndex = 0;
int sampleCount = 0;
unsigned long lastStatsTime = 0;

void IRAM_ATTR pulseCounter()
{
  unsigned long currentPulseTime = millis();

  if (currentPulseTime - lastPulseTime >= MIN_PULSE_INTERVAL)
  {
    pulseCount++;
    lastPulseTime = currentPulseTime;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("LoRa Sender");

  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(433E6))
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  pinMode(FLOW_METER, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_METER), pulseCounter, FALLING);

  previousTime = millis();
  lastStatsTime = millis();
}

void loop()
{
  unsigned long currentTime = millis();

  if (currentTime - previousTime >= 50)
  {
    noInterrupts();
    int pulses = pulseCount;
    unsigned long timeSinceLastPulse = currentTime - lastPulseTime;
    pulseCount = 0;
    interrupts();

    if (pulses > 0)
    {
      flowRate = (pulses / calibrationFactor) * 1200.0; // 60000ms/50ms = 1200
      totalVolume += (pulses / calibrationFactor);
    }
    else if (timeSinceLastPulse > FLOW_TIMEOUT)
    {
      flowRate = 0.0;
    }

    if (pulses > 0 || flowRate > 0)
    {
      Serial.print("Pulses: ");
      Serial.print(pulses);
      Serial.print(", Flow Rate: ");
      Serial.print(flowRate, 3);
      Serial.print(" L/min, Time since last pulse: ");
      Serial.print(timeSinceLastPulse);
      Serial.println(" ms");
    }

    flowRateSamples[sampleIndex] = flowRate;
    sampleIndex = (sampleIndex + 1) % MAX_SAMPLES;
    if (sampleCount < MAX_SAMPLES)
    {
      sampleCount++;
    }

    previousTime = currentTime;
  }

  if (currentTime - lastStatsTime < 1000)
    return;

  if (sampleCount > 0)
  {
    float sum = 0.0;
    for (int i = 0; i < sampleCount; i++)
      sum += flowRateSamples[i];
    float average = sum / sampleCount;

    float variance = 0.0;
    for (int i = 0; i < sampleCount; i++)
    {
      float diff = flowRateSamples[i] - average;
      variance += diff * diff;
    }
    float stdDev = sqrt(variance / sampleCount);

    LoRa.beginPacket();
    LoRa.write((uint8_t *)&currentTime, sizeof(currentTime));
    LoRa.write((uint8_t *)&average, sizeof(average));
    LoRa.write((uint8_t *)&stdDev, sizeof(stdDev));
    LoRa.endPacket();

    Serial.print("Binary packet sent - Timestamp: ");
    Serial.print(currentTime);
    Serial.print(", Average: ");
    Serial.print(average, 3);
    Serial.print(" L/min, Std Dev: ");
    Serial.print(stdDev, 3);
    Serial.print(" L/min, Packet size: 12 bytes, Samples: ");
    Serial.println(sampleCount);

    sampleIndex = 0;
    sampleCount = 0;
  }
  else
    Serial.println("No samples collected yet for statistics");

  lastStatsTime = currentTime;
}