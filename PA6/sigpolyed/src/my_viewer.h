# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

# include <sigogl/ui_check_button.h>
# include <sigogl/ui_slider.h>
# include <sigogl/ws_viewer.h>
#include <cmath>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvExit, EvViewCurveA, EvViewCurveB, EvViewCurveC, EvViewCurveD, EvViewCurveE, EvDeltaT };
	UiCheckButton *_viewA, *_viewB, *_viewC, * _viewD, * _viewE;
	UiSlider* _slider;
	SnPolyEditor* _polyed;
	SnLines2 * BezCurve;
	SnLines2* BSpline;
	SnLines2* CrSpline;

	//SnLines2* Bospline;


   public :
	   bool twoD = true;
	MyViewer ( int x, int y, int w, int h, const char* l );
	void build_ui ();
	void update_scene ();
	int factorial(int n);
	GsPnt2 eval_bezier(float t, const GsArray<GsPnt2>& P);
	GsPnt2 eval_bspline(float t, const GsArray<GsPnt2>& P);
	GsPnt2 crspline(float t, const GsArray<GsPnt2>& P);
	GsPnt2 bospline(float t, const GsArray<GsPnt2>& P);


	virtual int uievent ( int e ) override;
};

