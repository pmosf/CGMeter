#pragma once
#include <ISensor.h>
#include <HX711_ADC.h>
#include <DueFlashStorage.h>
#include <Log.h>

typedef struct {
	uint8_t validNumber;
	float factor;
} sg_cal_t;

class StrainGaugeSensor: public ISensor {
private:
	float value_;
	HX711_ADC *hx711_;
	sg_cal_t calibration_;
	float calibrationFactor_;
	int calAddr_;
	bool isCalibrated_;
	DueFlashStorage dueFlashStorage;
	bool sim_;
	float simValue_;

public:
	StrainGaugeSensor(float calFactor, int calAddr, int dout_pin, int sck_pin) {
		value_ = 0.;
		calAddr_ = calAddr;
		sim_ = false;
		simValue_ = 0;

		byte* b = dueFlashStorage.readAddress(calAddr_);
		memcpy(&calibration_, b, sizeof(calibration_));
		if (calibration_.validNumber == 0x55) {
			calibrationFactor_ = calibration_.factor;
			isCalibrated_ = true;
		} else {
			calibrationFactor_ = calFactor;
			isCalibrated_ = false;
		}

		hx711_ = new HX711_ADC(dout_pin, sck_pin);
	}

	virtual ~StrainGaugeSensor() {

	}

	void Start() {
		hx711_->begin();
		hx711_->start(2000);
		hx711_->setCalFactor(calibrationFactor_);
	}

	///Returns sensor name. Only for debug purpose
	const __FlashStringHelper* Name()
	{
		return F("StrainGaugeSensor");
	}

	///Return how many decimal places make sence for the sensor
	int Precision()
	{
		return 0;
	}

	///Return lowest possible measurement limit. If value outside of this limit, measurements threatet as erroneous
	float LowMeasurementLimit()
	{
		return -99999;
	}

	///Return highest possible measurement limit. If value outside of this limit, measurements threatet as erroneous
	float HighMeasurementLimit() {
		return 19000;
	}

	int Precission() {
		return 1;
	}

	bool Measure(float &data) {
		if (sim_) {
			value_ = simValue_;
			data = simValue_;
			return true;
		}

		hx711_->update();

		value_ = hx711_->getData();
		data = value_;

		return true;
	}

	bool Tare() {
		hx711_->tareNoDelay();
		return true;
	}

	bool getTareStatus() {
		return hx711_->getTareStatus();
	}

	bool Calibrate() {
		value_ = hx711_->getData();
		calibrationFactor_ = hx711_->getCalFactor();
		calibration_.validNumber = 0x55;
		calibration_.factor = calibrationFactor_;

		byte b[sizeof(calibration_)];
		memcpy(b, &calibration_, sizeof(calibration_));
		//dueFlashStorage.write(calAddr_, b, sizeof(calibration_));

		return true;
	}

	void SetMode(bool sim, float simVal) {
		sim_ = sim;
		simValue_ = simVal;
	}
};
