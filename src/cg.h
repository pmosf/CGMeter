#pragma once
#include <Window.h>
#include "plane.h"

class Cg: public Window, public ISensorHasDataEventReceiver {
	TextBoxNumber* txtActualCg_;
	TextBoxNumber* txtWeigth_;
	Plane *plane_;

	void initLabel(Label *label, const __FlashStringHelper *font) {
		if (font != nullptr) {
			label->SetFont(font);
		}
		AddChild(label);
	}

	void initTextWindow(TextBoxNumber *wnd, bool isReadOnly) {
		wnd->SetDecorators(GetDecorators()); // here we save one decorator beacuse main window and text window have thae same decorator properties: black background
		wnd->SetFont(F("Retro"));
		wnd->SetMargins(5, 5);
		wnd->SetPrecission(1);
		wnd->SetIsReadOnly(isReadOnly);
		wnd->SetHorizontalAlignment(DC::Right);
		AddChild(wnd);
	}

	void NotifySensorHasData(SensorManager *sensorManager) {
		txtActualCg_->SetNumber(plane_->GetCgLocation());
		txtWeigth_->SetNumber(plane_->GetBallast());
	}

public:
	Cg(Plane *plane, const __FlashStringHelper * name, int left, int top,
			int width, int height) :
			Window(name, left, top, width, height), plane_(plane) {
		int column_1_x = 5;
		int column_2_x = 200;
		int column_3_x = 293;
		int wnd_height = 25;
		int wnd_height_space = 2;
		int row_y = 4;
		int childIdx = 0;

		SetDecorators(Environment::Get()->FindDecorators(F("Window")));

		initLabel(new Label(0, 0, 0, 0, F("Measurements:")), F("Retro"));
		Children()[childIdx++]->Move(0, row_y, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;

		//
		initLabel(new Label(0, 0, 0, 0, F("Actual CG")), F("Retro"));
		Children()[childIdx++]->Move(column_1_x, row_y + 5, 50, wnd_height);
		txtActualCg_ = new TextBoxNumber(0, 0, 0, 0, 0);
		txtActualCg_->SetNumber(0);
		initTextWindow(txtActualCg_, true);
		txtActualCg_->SetPrecission(0);
		Children()[childIdx++]->Move(column_2_x, row_y, 90, wnd_height);
		initLabel(new Label(0, 0, 0, 0, F("mm")), F("Retro"));
		Children()[childIdx++]->Move(column_3_x, row_y + 5, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;

		//
		initLabel(new Label(0, 0, 0, 0, F("Weight to be added")), F("Retro"));
		Children()[childIdx++]->Move(column_1_x, row_y + 5, 50, wnd_height);
		txtWeigth_ = new TextBoxNumber(0, 0, 0, 0, 0);
		txtWeigth_->SetNumber(0);
		initTextWindow(txtWeigth_, true);
		txtWeigth_->SetPrecission(0);
		Children()[childIdx++]->Move(column_2_x, row_y, 90, wnd_height);
		initLabel(new Label(0, 0, 0, 0, F("g")), F("Retro"));
		Children()[childIdx++]->Move(column_3_x, row_y + 5, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;

		plane_->RegisterHasDataEventReceiver(this);
	}

	virtual ~Cg() {
	}
};
