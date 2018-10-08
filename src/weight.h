#pragma once
#include <Window.h>
#include <Label.h>
#include <TextBoxNumber.h>
#include <GaugeBar.h>
#include <RadioButton.h>
#include <TimerSensorManager.h>
#include <Dialog.h>
#include <Log.h>

#include "plane.h"
#include "Calibration.h"

class Weight: public Window,
		public ITouchEventReceiver,
		public ISensorHasDataEventReceiver {

private:
	static const int nbTxtBox_ = 4;
	static const int nbLabel_ = 8;
	TextBoxNumber* txtWeigth_[nbTxtBox_];
	Button *btnCal_;
	Button *btnTare_;
	Plane *plane_;
	enum TextBoxIndex {
		NoseTail = Plane::NoseTail,
		MainLeft = Plane::MainLeft,
		MainRight = Plane::MainRight,
		Total
	};

public:
	Weight(Plane* plane, const __FlashStringHelper * name, int left, int top,
			int width, int height) :
			Window(name, left, top, width, height) {
		plane_ = plane;

		SetDecorators(Environment::Get()->FindDecorators(F("Window")));

		initLabel(new Label(0, 0, 0, 0, F("Nose/Tail")));
		initLabel(new Label(0, 0, 0, 0, F("g")));
		initLabel(new Label(0, 0, 0, 0, F("Main Left")));
		initLabel(new Label(0, 0, 0, 0, F("g")));
		initLabel(new Label(0, 0, 0, 0, F("Main Right")));
		initLabel(new Label(0, 0, 0, 0, F("g")));
		initLabel(new Label(0, 0, 0, 0, F("Total")));
		initLabel(new Label(0, 0, 0, 0, F("g")));

		for (int i = 0; i < nbTxtBox_; i++) {
			txtWeigth_[i] = new TextBoxNumber(0, 0, 0, 0, 0);
			txtWeigth_[i]->SetNumber(0);
			initTextWindow(txtWeigth_[i], true);
			txtWeigth_[i]->SetPrecission(0);
		}

		btnCal_ = new Button(0, 0, 0, 0, F("Cal"));
		initButton(btnCal_);
		btnCal_->SetMargins(0, 20);
		btnTare_ = new Button(0, 0, 0, 0, F("Tare"));
		initButton(btnTare_);
		btnTare_->SetMargins(0, 20);

		Initialize();

		plane->RegisterHasDataEventReceiver(this);
	}

	virtual ~Weight() {

	}

private:
	///Initilizes positions of child windows + timers
	void Initialize() {
		const static int column_1_x = 5;
		const static int column_2_x = 200;
		const static int column_3_x = 293;
		const static int wnd_height = 25;
		const static int wnd_height_space = 2;
		int row_y[nbTxtBox_ + nbLabel_];

		for (int i = 0; i < nbTxtBox_ + nbLabel_; ++i) {
			if (i == 0) {
				row_y[0] = 5;
			} else {
				row_y[i] = row_y[i - 1] + wnd_height + wnd_height_space;
			}
		}

		for (int i = 0; i < nbTxtBox_; ++i) {
			txtWeigth_[i]->Move(column_2_x, row_y[i], 90, wnd_height);
		}

		Children()[0]->Move(column_1_x, row_y[0] + 5, 50, wnd_height);
		Children()[1]->Move(column_3_x, row_y[0] + 5, 50, wnd_height);
		Children()[2]->Move(column_1_x, row_y[1] + 5, 50, wnd_height);
		Children()[3]->Move(column_3_x, row_y[1] + 5, 50, wnd_height);
		Children()[4]->Move(column_1_x, row_y[2] + 5, 50, wnd_height);
		Children()[5]->Move(column_3_x, row_y[2] + 5, 50, wnd_height);
		Children()[6]->Move(column_1_x, row_y[3] + 5, 50, wnd_height);
		Children()[7]->Move(column_3_x, row_y[3] + 5, 50, wnd_height);

		btnCal_->Move(5, 150, 135, 50);
		btnTare_->Move(172, 150, 135, 50);
	}

	void initLabel(Label *label) {
		label->SetFont(F("Retro"));
		AddChild(label);
	}
	void initRadio(RadioButton *radio) {
		radio->SetMargins(20, 5);
		radio->RegisterTouchEventReceiver(this);
		AddChild(radio);
	}
	void initTextWindow(TextBoxNumber *wnd, bool isReadOnly) {
		wnd->SetDecorators(GetDecorators()); // here we save one decorator beacuse main window and text window have thae same decorator properties: black background
		wnd->SetFont(F("Retro"));
		wnd->SetMargins(5, 5);
		wnd->SetPrecission(0);
		wnd->SetIsReadOnly(isReadOnly);
		wnd->SetHorizontalAlignment(DC::Right);
		AddChild(wnd);
	}
	void initButton(Button *btn) {
		btn->RegisterTouchEventReceiver(this);
		AddChild(btn);
	}

	///Events routing for gui interaction (see RegisterTouchEventReceiver and public ITouchEventReceiver declaration)
	void NotifyTouch(Window *window) {
		if (window == btnCal_) {
			return;
		}

		if (window == btnTare_) {
			plane_->Tare();
			return;
		}
	}

	///If sensor data was changed this notification is call
	void NotifySensorHasData(SensorManager *sensorManager) {
		if (!plane_->GetTareStatus()) {
			txtWeigth_[TextBoxIndex::NoseTail]->SetNumber(-9999);
			txtWeigth_[TextBoxIndex::MainLeft]->SetNumber(-9999);
			txtWeigth_[TextBoxIndex::MainRight]->SetNumber(-9999);
			txtWeigth_[TextBoxIndex::Total]->SetNumber(-9999);
		} else {
			switch (plane_->GetUpdated(sensorManager)) {
			case Plane::NoseTail:
			case Plane::MainLeft:
			case Plane::MainRight:
				txtWeigth_[TextBoxIndex::NoseTail]->SetNumber(
						plane_->GetWeightValues().noseTail);
				txtWeigth_[TextBoxIndex::MainLeft]->SetNumber(
						plane_->GetWeightValues().mainLeft);
				txtWeigth_[TextBoxIndex::MainRight]->SetNumber(
						plane_->GetWeightValues().mainRight);
				txtWeigth_[TextBoxIndex::Total]->SetNumber(
						plane_->GetWeightValues().total);
				break;
			default:
				break;
			}
		}
	}
};
