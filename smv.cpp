/*
 * smv.cpp
 *
 *  Created on: Sep 22, 2012
 *      Author: brzeszczot
 */

#include "smv.hpp"

SMV::SMV()
{
	//	void runTimerEvent()
	//{
	//	maWidgetScreenShow(0);
	//}
	//TimerListener
	//addTimer(this,6000,1);
	engine = new Engine();
	createUI();
}

SMV::~SMV()
{
	maLocationStop();
	delete engine, mScreen, mList;
}

void SMV::createUI()
{
	maLabel1 = new Label();
	maLabel1->fillSpaceHorizontally();
	maLabel1->setText("Wawa 1935");

	mLayout = new VerticalLayout();
	mLayout->fillSpaceHorizontally();
	mLayout->wrapContentVertically();

	mScreen = new Screen();
	mButton = new Button();
	mButton->fillSpaceHorizontally();
	mButton->setText("Start");
	mButton->addButtonListener(this);

	mEditBox = new EditBox();
	mEditBox->fillSpaceHorizontally();
	mEditBox->setText(PATH);

	maLabel = new Label();
	maLabel->fillSpaceHorizontally();
	maLabel->setText("brzeszczot@gmail.com   (2012)");

	//mList = new ListView();
	//mList->fillSpaceHorizontally();
	//mList->wrapContentVertically();

	mLayout->addChild(maLabel1);
	mLayout->addChild(mEditBox);
	mLayout->addChild(mButton);
	mLayout->addChild(maLabel);
	mScreen->addChild(mLayout);
	/*
	mScreen->setMainWidget(mEditBox);
	mScreen->setMainWidget(mList);

	for(int ii=0;ii<engine->how_many_dir;ii++)
	{
		ListViewItem *listItem = new ListViewItem();
		listItem->fillSpaceHorizontally();
		listItem->wrapContentVertically();
		//listItem->setText(engine->dir_names[ii]);
		mList->addChild(listItem);
	}
	mList->addListViewListener((ListViewListener*) this);
	*/
	mScreen->show();
}

void SMV::buttonClicked(Widget* button)
{
	mEditBox->hideKeyboard();

	MAUtil::String str;
	str = mEditBox->getText();
	strcpy(engine->url_path, str.c_str());
	int str_c = strlen(engine->url_path);
	if(engine->url_path[str_c-1] != '/')
	{
		engine->url_path[str_c++] = '/';
		engine->url_path[str_c] = '\0';
		//mEditBox->setText(engine->url_path);
	}

	MAHandle directory = maFileOpen(engine->url_path, MA_ACCESS_READ);
	if (directory < 0)
		maMessageBox("Uwaga!", "Blad FileOpen!");
	else
	{
		if (!maFileExists(directory))
			maMessageBox("Uwaga!", "Katalog nie istnieje!");
		else
		{
			maWidgetScreenShow(0);
			int res = maLocationStart();
			if(res<0)	maPanic(1, "No GPS available");
			engine->read_conf_file();
			engine->env_init();
			engine->draw();
		}
	}
}

void SMV::customEvent(const MAEvent& event)
{
	if(event.type == EVENT_TYPE_LOCATION)
	{
	    MALocation& loc = *(MALocation*)event.data;
	    engine->gps = &loc;
	    if(maGetMilliSecondCount()-milisec>2000)
	    	engine->center_view = true;
	    engine->draw();
	}
	else if(event.type == EVENT_TYPE_LOCATION_PROVIDER)
	{
		const char *strings[]= {"AVAILABLE","TEMPORARILY_UNAVAILABLE","OUT_OF_SERVICE"};
		//maPanic(1, "No GPS available");
		//printf("gps provider: %s\n", strings[event.state-1]);
	}
}

void SMV::keyPressEvent(int keyCode, int nativeCode)
{
	if (MAK_BACK == keyCode)
	{
		close();
		// za cholere nie da sie powrocic do nativeui (powraca ale nie reaguje) - dlatego za kazdym razem od nowa generuje listview
		/*
		maLocationStop();
		delete engine, mScreen, mList;

		// zacznij od nowa:
		engine = new Engine();
		createUI();
		*/
	}
	if(MAK_0 == keyCode)
	{
		close();
	}
}

void SMV::pointerPressEvent(MAPoint2d point)
{
	if(point.x>=engine->gPointer.x && point.y>=engine->gPointer.y)
	{
		engine->gPointer2.x = point.x - engine->gPointer.x;
		engine->gPointer2.y = point.y - engine->gPointer.y;
	}
}

void SMV::pointerMoveEvent(MAPoint2d point)
{
	engine->gPointer.x = point.x - engine->gPointer2.x;
	engine->gPointer.y = point.y - engine->gPointer2.y;
	engine->center_view = false;
	milisec = maGetMilliSecondCount();
	engine->draw();
}

void SMV::pointerReleaseEvent(MAPoint2d point)
{
	milisec = maGetMilliSecondCount();
}

void SMV::listViewItemClicked(ListView* listView, int index)
{
	maWidgetScreenShow(0);
	int res = maLocationStart();
	if(res<0)	maPanic(1, "No GPS available");
	engine->selected_dir = index;
	engine->read_conf_file();
	engine->env_init();
	engine->draw();
}

void SMV::listViewItemClicked(ListView* listView, ListViewItem* listViewItem)
{
	/*
	if(listView==mList)
	mList->removeChild(listViewItem);
	*/
}
