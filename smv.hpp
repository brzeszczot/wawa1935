/*
 * smv.hpp
 *
 *  Created on: Sep 22, 2012
 *      Author: brzeszczot
 */

#ifndef SMV_HPP_
#define SMV_HPP_

#include <ma.h>
#include <mavsprintf.h>
#include <MAUtil/Moblet.h>
#include <NativeUI/Widgets.h>
#include <NativeUI/WidgetUtil.h>
#include <string>

#include "engine.hpp"
#define PATH "/mnt/sdcard/Download/smv_res/"

using namespace MAUtil;
using namespace NativeUI;

class SMV: public Moblet, public ListViewListener, public ButtonListener
{
	public:
		SMV();
		virtual ~SMV();
		void createUI();
		void customEvent(const MAEvent& event);
		void keyPressEvent(int keyCode, int nativeCode);
		virtual void pointerPressEvent(MAPoint2d point);
		virtual void pointerMoveEvent(MAPoint2d point);
		virtual void pointerReleaseEvent(MAPoint2d point);
		virtual void listViewItemClicked(ListView* listView, ListViewItem* listViewItem);
		virtual void listViewItemClicked(ListView* listView, int index);
		virtual void buttonClicked(Widget* button);
	private:
		int milisec;
		Engine *engine;
		Screen* mScreen;
		ListView *mList;
		EditBox* mEditBox;
		Button* mButton;
		Label* maLabel;
		VerticalLayout* mLayout;
};

#endif /* SMV_HPP_ */
