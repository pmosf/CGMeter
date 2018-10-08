#pragma once
#include <Window.h>
#include "plane.h"

class Setup: public Window, IContentChangedEventReceiver {
	TextBoxNumber* txtTailNose_;
	TextBoxNumber* txtMainLeft_;
	TextBoxNumber* txtMainRight_;
	TextBoxNumber* txtDesiredCg_;
	TextBoxNumber* txtBallast_;

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
		wnd->RegisterContentChangedReceiver(this);
		AddChild(wnd);
	}

	void NotifyContentChanged(Window *textBox) {
		if (textBox == txtTailNose_) {
			plane_->CgSettings().tailNoseDistance = txtTailNose_->GetNumber();
		}

		else if (textBox == txtMainLeft_) {
			plane_->CgSettings().mainLeftDistance = txtMainLeft_->GetNumber();
		}

		else if (textBox == txtMainRight_) {
			plane_->CgSettings().mainRightDistance = txtMainRight_->GetNumber();
		}

		else if (textBox == txtDesiredCg_) {
			if (txtDesiredCg_->GetNumber() == txtBallast_->GetNumber()) {
				txtDesiredCg_->SetDecorators(
						Environment::Get()->FindDecorators(F("RedRectangle")));
				txtBallast_->SetDecorators(
						Environment::Get()->FindDecorators(F("RedRectangle")));
			} else {
				txtDesiredCg_->SetDecorators(
						Environment::Get()->FindDecorators(F("EditTextBox")));
				txtBallast_->SetDecorators(
						Environment::Get()->FindDecorators(F("EditTextBox")));
			}

			plane_->CgSettings().desiredCgDistance = txtDesiredCg_->GetNumber();
		}

		else if (textBox == txtBallast_) {
			if (txtDesiredCg_->GetNumber() == txtBallast_->GetNumber()) {
				txtDesiredCg_->SetDecorators(
						Environment::Get()->FindDecorators(F("RedRectangle")));
				txtBallast_->SetDecorators(
						Environment::Get()->FindDecorators(F("RedRectangle")));
			} else {
				txtDesiredCg_->SetDecorators(
						Environment::Get()->FindDecorators(F("EditTextBox")));
				txtBallast_->SetDecorators(
						Environment::Get()->FindDecorators(F("EditTextBox")));
			}

			plane_->CgSettings().ballast = txtBallast_->GetNumber();
		}

		plane_->ForceUpdate();
	}

public:
	Setup(Plane *plane, const __FlashStringHelper * name, int left, int top,
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

		//
		initLabel(new Label(0, 0, 0, 0, F("Enter distances:")), F("Retro"));
		Children()[childIdx++]->Move(0, row_y, 50, wnd_height);
		row_y += wnd_height;

		//
		initLabel(new Label(0, 0, 0, 0, F("Tail/Nose")), F("Retro"));
		Children()[childIdx++]->Move(column_1_x, row_y + 5, 50, wnd_height);
		txtTailNose_ = new TextBoxNumber(0, 0, 0, 0, 0);
		txtTailNose_->SetNumber(0);
		initTextWindow(txtTailNose_, false);
		txtTailNose_->SetPrecission(0);
		Children()[childIdx++]->Move(column_2_x, row_y, 90, wnd_height);
		initLabel(new Label(0, 0, 0, 0, F("mm")), F("Retro"));
		Children()[childIdx++]->Move(column_3_x, row_y + 5, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;

		//
		initLabel(new Label(0, 0, 0, 0, F("Main Left")), F("Retro"));
		Children()[childIdx++]->Move(column_1_x, row_y + 5, 50, wnd_height);
		txtMainLeft_ = new TextBoxNumber(0, 0, 0, 0, 0);
		txtMainLeft_->SetNumber(0);
		initTextWindow(txtMainLeft_, false);
		txtMainLeft_->SetPrecission(0);
		Children()[childIdx++]->Move(column_2_x, row_y, 90, wnd_height);
		initLabel(new Label(0, 0, 0, 0, F("mm")), F("Retro"));
		Children()[childIdx++]->Move(column_3_x, row_y + 5, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;

		//
		initLabel(new Label(0, 0, 0, 0, F("Main Right")), F("Retro"));
		Children()[childIdx++]->Move(column_1_x, row_y + 5, 50, wnd_height);
		txtMainRight_ = new TextBoxNumber(0, 0, 0, 0, 0);
		txtMainRight_->SetNumber(0);
		initTextWindow(txtMainRight_, false);
		txtMainRight_->SetPrecission(0);
		Children()[childIdx++]->Move(column_2_x, row_y, 90, wnd_height);
		initLabel(new Label(0, 0, 0, 0, F("mm")), F("Retro"));
		Children()[childIdx++]->Move(column_3_x, row_y + 5, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;

		initLabel(new Label(0, 0, 0, 0, F("Desired CG")), F("Retro"));
		Children()[childIdx++]->Move(column_1_x, row_y + 5, 50, wnd_height);
		txtDesiredCg_ = new TextBoxNumber(0, 0, 0, 0, 0);
		txtDesiredCg_->SetNumber(0);
		initTextWindow(txtDesiredCg_, false);
		txtDesiredCg_->SetPrecission(0);
		Children()[childIdx++]->Move(column_2_x, row_y, 90, wnd_height);
		initLabel(new Label(0, 0, 0, 0, F("mm")), F("Retro"));
		Children()[childIdx++]->Move(column_3_x, row_y + 5, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;

		initLabel(new Label(0, 0, 0, 0, F("Ballast")), F("Retro"));
		Children()[childIdx++]->Move(column_1_x, row_y + 5, 50, wnd_height);
		txtBallast_ = new TextBoxNumber(0, 0, 0, 0, 0);
		txtBallast_->SetNumber(0);
		initTextWindow(txtBallast_, false);
		txtBallast_->SetPrecission(0);
		Children()[childIdx++]->Move(column_2_x, row_y, 90, wnd_height);
		initLabel(new Label(0, 0, 0, 0, F("mm")), F("Retro"));
		Children()[childIdx++]->Move(column_3_x, row_y + 5, 50, wnd_height);
		row_y += wnd_height + wnd_height_space;
	}

	virtual ~Setup() {

	}
};
