#include "Arduino.h"
#include <Log.h>
#include <LinkedList.h>
#include <WindowsManager.h>
#include <DefaultDecorators.h>
#include <TabControl.h>
#include <SensorManager.h>
#include <MeasurementNode.h>
#include <DC_UTFT.h>
#include <plane.h>
#include <TouchUTFT.h>
#include <TabControl.h>

#include "weight.h"
#include "setup.h"
#include "cg.h"
#include "StrainGaugeSensor.h"
#include "plane.h"
#include "InitWindow.h"

UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);
URTouch myTouch(6, 5, 4, 3, 2);
DC_UTFT dc(&myGLCD);
TouchUTFT touch(&myTouch);

//list where all sensors are collected
LinkedList<SensorManager> sensors;

//manager which controls the measurement process
MeasurementNode measurementNode(sensors, nullptr);

//manager which is responsible for window updating process
WindowsManager<MainWindow> windowsManager(&dc, &touch);

extern uint8_t Retro8x16[];

Plane plane;

void setup() {
	//setup log (out is wrap about Serial class)
	out.begin(9600);
	out << F("Setup") << endln;

	//initialize display
	myGLCD.InitLCD();
	myGLCD.clrScr();

	//initialize touch
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);

	DC_UTFT::RegisterDefaultFonts();
	Environment::Get()->RegisterFont(new AFontUTFT(F("Retro"), Retro8x16));
	DefaultDecorators::InitAll();

	// initialize plane
	out << F("Initializing plane object") << endln;
	plane.Initialize(sensors);

	//initialize window manager
	out << F("Initializing windowsManager object") << endln;
	windowsManager.Initialize();

	// create InitWindow
	InitWindow *initWnd = new InitWindow(F("InitWindow"), 0, 0, 0, 0);
	windowsManager.MainWnd()->AddChild(initWnd);

	// create tab control
	TabControl *tabCtrl = new TabControl(F("TabControl"), 2, 2,
			windowsManager.MainWnd()->Width() - 4,
			windowsManager.MainWnd()->Height() - 4);

	tabCtrl->SetVisible(false);

	windowsManager.MainWnd()->AddChild(tabCtrl);

	//create tabs
	Weight *tabWeight = new Weight(&plane, F("Weight"), 0, 0, 0, 0);
	Setup *tabSetup = new Setup(&plane, F("Setup"), 0, 0, 0, 0);
	Cg *tabCg = new Cg(&plane, F("CG"), 0, 0, 0, 0);

	tabCtrl->AddTab(F("Weight"), static_cast<Window*>(tabWeight));
	tabCtrl->AddTab(F("Setup"), static_cast<Window*>(tabSetup));
	tabCtrl->AddTab(F("CG"), static_cast<Window*>(tabCg));

	windowsManager.loop();

	plane.Start();

	initWnd->SetVisible(false);
	tabCtrl->SetVisible(true);

	DecoratorList *redDecorator = new DecoratorList();
	redDecorator->Add(new DecoratorRectFill(Color::Red, false));
	redDecorator->Add(new DecoratorColor(Color::Black));
	Environment::Get()->RegisterDecoratorsGroup(F("RedRectangle"), redDecorator);

	out << F("End setup") << endln;
}

void loop() {
	if (measurementNode.Measure()) {
		//following if is only for debugging purposes
		if (measurementNode.IsChanged()) {
			measurementNode.LogResults();
		}
	}
	//give window manager an opportunity to update display
	windowsManager.loop();
}
