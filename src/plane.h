#pragma once

#include "TimerSensorManager.h"
#include "StrainGaugeSensor.h"

class Plane: public ISensorHasDataEventReceiver {
public:
	struct CgSettings {
		float mainLeftDistance;
		float mainRightDistance;
		float tailNoseDistance;
		float desiredCgDistance;
		float ballast;
	};

	struct WeigthValues {
		float total;
		float mainLeft;
		float mainRight;
		float noseTail;
	};

	struct MomentValues {
		float mainLeft;
		float mainRight;
		float noseTail;
		float total;
	};

	enum UpdatedStrainGauge {
		NoseTail, MainLeft, MainRight, None
	};

private:
	bool isInitialized_;
	static const uint32_t nbStrainGauge_ = 3;
	CgSettings cgSettings_;
	WeigthValues weightValues_;
	MomentValues momentValues_;
	SensorManager *mainLeftGauge_;
	SensorManager *mainRightGauge_;
	SensorManager *noseTailGauge_;
	LinkedList<ISensorHasDataEventReceiver> hasDataEventsReceiver_;
	float cgLocation_;
	float ballast_;
	StrainGaugeSensor *gauge_[nbStrainGauge_];
	bool isTareOK_;
	bool forceUpdate_;

public:
	Plane() {
		isInitialized_ = false;
		mainLeftGauge_ = nullptr;
		mainRightGauge_ = nullptr;
		noseTailGauge_ = nullptr;
		memset(&cgSettings_, 0, sizeof(cgSettings_));
		memset(&weightValues_, 0, sizeof(weightValues_));
		memset(&momentValues_, 0, sizeof(momentValues_));
		cgLocation_ = 0;
		ballast_ = 0;
		isTareOK_ = true;
		forceUpdate_ = false;
	}

	virtual ~Plane() {

	}

	void Initialize(LinkedList<SensorManager> &sensors) {
		uint32_t sg_cal_addr = 0;

		// left gauge
		mainLeftGauge_ = new SensorManager(
				new StrainGaugeSensor(119, sg_cal_addr, 9, 8), -10., 19.5, 350,
				false);
		sg_cal_addr += sizeof(sg_cal_t);
		mainLeftGauge_->initSecondsBuffer(1000);
		sensors.Add(mainLeftGauge_);
		mainLeftGauge_->RegisterHasDataEventReceiver(this);

		// right gauge
		mainRightGauge_ = new SensorManager(
				new StrainGaugeSensor(116, sg_cal_addr, 11, 10), -10., 19.5, 350,
				false);
		sg_cal_addr += sizeof(sg_cal_t);
		mainRightGauge_->initSecondsBuffer(1000);
		sensors.Add(mainRightGauge_);
		mainRightGauge_->RegisterHasDataEventReceiver(this);

		// nose/tail gauge
		noseTailGauge_ = new SensorManager(
				new StrainGaugeSensor(125, sg_cal_addr, 13, 12), -10., 19.5, 350,
				false);
		sg_cal_addr += sizeof(sg_cal_t);
		noseTailGauge_->initSecondsBuffer(1000);
		sensors.Add(noseTailGauge_);
		noseTailGauge_->RegisterHasDataEventReceiver(this);

		gauge_[0] = static_cast<StrainGaugeSensor *>(mainLeftGauge_->Sensor());
		gauge_[1] = static_cast<StrainGaugeSensor *>(mainRightGauge_->Sensor());
		gauge_[2] = static_cast<StrainGaugeSensor *>(noseTailGauge_->Sensor());

		/*gauge_[0]->SetMode(true, 100);
		gauge_[1]->SetMode(true, 100);
		gauge_[2]->SetMode(true, 10);*/

		isInitialized_ = true;
	}

	void Start() {
		for (int i = 0; i < nbStrainGauge_; ++i)
			gauge_[i]->Start();
	}

	void RegisterHasDataEventReceiver(ISensorHasDataEventReceiver *receiver) {
		hasDataEventsReceiver_.Add(receiver);
	}

	CgSettings & CgSettings() {
		return cgSettings_;
	}

	WeigthValues & GetWeightValues() {
		return weightValues_;
	}

	float GetCgLocation() {
		return cgLocation_;
	}

	float GetBallast() {
		return ballast_;
	}

	void ForceUpdate() {
		forceUpdate_ = true;
		NotifySensorHasData(nullptr);
	}

	bool Tare() {
		bool done = false;

		for (int i = 0; i < 3; ++i)
			gauge_[i]->Tare();

		return done;
	}

	bool GetTareStatus() {
		isTareOK_ = true;

		for (int i = 0; i < 3; ++i)
			isTareOK_ &= gauge_[i]->getTareStatus();

		return isTareOK_;
	}

	bool Calibrate() {
		bool done = true;

		return done;
	}

	void ProcessData() {

		weightValues_.total = weightValues_.mainLeft + weightValues_.mainRight
				+ weightValues_.noseTail;

		momentValues_.total = momentValues_.mainLeft + momentValues_.mainRight
				+ momentValues_.noseTail;

		if (weightValues_.total > 0 && isTareOK_) {
			cgLocation_ = momentValues_.total / weightValues_.total;
		} else
			cgLocation_ = 0;

		ballast_ = weightValues_.total
				* (cgLocation_ - cgSettings_.desiredCgDistance)
				/ (cgSettings_.desiredCgDistance - cgSettings_.ballast);
	}

	void NotifySensorHasData(SensorManager *sensorManager) {
		if (sensorManager == mainLeftGauge_ || forceUpdate_) {
			weightValues_.mainLeft = mainLeftGauge_->GetData();
			if (weightValues_.mainLeft < 0)
				weightValues_.mainLeft = 0;

			momentValues_.mainLeft = cgSettings_.mainLeftDistance
					* weightValues_.mainLeft;
		}

		if (sensorManager == mainRightGauge_ || forceUpdate_) {
			weightValues_.mainRight = mainRightGauge_->GetData();
			if (weightValues_.mainRight < 0)
				weightValues_.mainRight = 0;

			momentValues_.mainRight = cgSettings_.mainRightDistance
					* weightValues_.mainRight;
		}

		if (sensorManager == noseTailGauge_ || forceUpdate_) {
			weightValues_.noseTail = noseTailGauge_->GetData();
			if (weightValues_.noseTail < 0)
				weightValues_.noseTail = 0;

			momentValues_.noseTail = cgSettings_.tailNoseDistance
					* weightValues_.noseTail;
		}

		forceUpdate_ = false;

		ProcessData();

		for (int i = 0; i < hasDataEventsReceiver_.Count(); i++)
			hasDataEventsReceiver_[i]->NotifySensorHasData(sensorManager);
	}

	UpdatedStrainGauge GetUpdated(SensorManager *sensorManager) {
		if (sensorManager == mainLeftGauge_) {
			return MainLeft;
		}
		if (sensorManager == mainRightGauge_) {
			return MainRight;
		}
		if (sensorManager == noseTailGauge_) {
			return NoseTail;
		}

		return None;
	}
};
#if 0
TGW x CG offset / CG to Ballast
= Amount of Weight needed at ballast_ location

Where:
TGW = Aircraft Total Gross Weight times:
CG Offset = Distance between current CG and desired CG
Divided by:
CG to Ballast = Distance between desired CG and desired ballast_ location
#endif
