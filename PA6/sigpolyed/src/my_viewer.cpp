
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>

static void my_polyed_callback ( SnPolyEditor* pe, enum SnPolyEditor::Event e, int pid )
{
	MyViewer* v = (MyViewer*)pe->userdata();
	if ( e==SnPolyEditor::PostMovement || e==SnPolyEditor::PostEdition || e==SnPolyEditor::PostInsertion )
	{	v->update_scene ();
	}
}

MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	rootg()->add ( _polyed = new SnPolyEditor );
	rootg()->add ( BezCurve = new SnLines2 );
	rootg()->add(BSpline = new SnLines2);
	rootg()->add(CrSpline = new SnLines2);

	BezCurve->color ( GsColor(20,200,25) );
	BezCurve->line_width ( 2.0f );

	BSpline->color(GsColor(75, 10, 200));
	BSpline->line_width(2.0f);

	CrSpline->color(GsColor(200, 50, 10));
	CrSpline->line_width(2.0f);

	// set initial control polygon:
	_polyed->callback ( my_polyed_callback, this );
	_polyed->max_polygons (1);
	_polyed->solid_drawing (0);
	GsPolygon& P = _polyed->polygons()->push();
	P.setpoly ( "-2 -2  -1 1  1 0  2 -2" );
	P.open(true);

	// start:
	build_ui ();
	update_scene ();
	message() = "Click on polygon to edit, use Esc to switch edition mode, Del deletes selected points. Enjoy!";
}

void MyViewer::build_ui ()
{
	UiPanel *p;
	p = uim()->add_panel ( "", UiPanel::HorizLeft );
	p->add(_viewE = new UiCheckButton("Bessel-Overhauser", EvViewCurveE, true));
	p->add(_viewD = new UiCheckButton("Catmull-Rom Spline", EvViewCurveD, true));
	p->add( _viewC = new UiCheckButton("BSpline Curve", EvViewCurveC, true));
	p->add ( _viewA=new UiCheckButton ( "Bezier Curve", EvViewCurveA, true ) );
	p->add ( _viewB=new UiCheckButton ( "CurveB", EvViewCurveB, true ) );

	p->add ( _slider=new UiSlider ( " dt:", EvDeltaT, 0,0,150 ) );
	_slider->separate();
	_slider->range ( 0.01f, 0.25f );

	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}


int MyViewer:: factorial(int n)
{
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

//P is an array containing control points
//i is the point
//t is the slider value
//n is the order
//bezier of order n for given n = P.size()-1 control points, t in [0,1]
GsPnt2 MyViewer::eval_bezier(float t, const GsArray<GsPnt2>& P){
//	GsPnt2 point = P[0] * (1.0f - t) + P.top() * t; // add the correct equations here

	GsVec2 temp_point, final_point;
	int n_fact, i_fact, ni_fact = 0;
	float  B, C = 0;
	int n = P.size()-1;

	for (int i = 0; i <= n; i++) {
		n_fact = gs_fact(n);
		i_fact = gs_fact(i);
		ni_fact = gs_fact(n-i);

		C =((float)n_fact / ((float)i_fact * (float)ni_fact));
		B = (C * powf(t, (float)i)) * powf(1.0f - t, (float)(n -i));
		temp_point = P[i] * B;
		final_point += temp_point;
	}
	return final_point;
}


//B-Spline order k, n=p.size()-1
//for order k = 3(degree 2, quadratic case)
//i is slider val
//u is the knot values

static float N(int i, int k, float u) {
	float ui = float(i);
	if (k == 1) 
		return ui <= u && u < ui + 1 ? 1.0f : 0;
	else
		return ((u - ui) / (k - 1))* N(i, k - 1, u) + ((ui + k - u) / (k - 1)) * N(i + 1, k - 1, u);
}

GsPnt2 MyViewer::eval_bspline(float t, const GsArray<GsPnt2>& P){
	//u is t
	GsPnt2 point, finalpoint;
	int n = P.size();

	for (int i = 0; i < n; i++) {

		point = P[i] * N(i, 3, t);
		finalpoint += point;
	}
	return finalpoint;
}

//evaluates catmull-rom cubic spline, n=p.size()-1 t in [0,n-2]
//t is slider val
GsPnt2 MyViewer::crspline(float t, const GsArray<GsPnt2>& P) {
	GsPnt2 point, I, I_neg, I_pos, I_plus,finalpoint;
	GsArray<GsPnt2> computed;
	//int n = P.size()-1;

		int i = int (floor(t+1));
		float t2 = t - float(floor(t));
		int j = i + 1;
		I = (P[i + 1] - P[i - 1]) / 2;
		I_plus = (P[j + 1] - P[j - 1]) / 2;
		I_neg = P[j] - (I_plus / 3);
		I_pos = P[i] + (I / 3);
		computed.push(P[i]);
		computed.push(I_pos);
		computed.push(I_neg);
		computed.push(P[j]);
	point = eval_bezier(t2, computed);
	return point;

}

//evaluates a bessel over-hauser spline, n=p.size()-1, t in [0,n-2]
GsPnt2 MyViewer::bospline(float t, const GsArray<GsPnt2>& P) {
	GsPnt2 point, vi_pos, vi_neg, vi, pi_neg, pi_pos;
	int i = int(floor(t + 1));
	int j = i + 1;

	//vi_pos = (P[j] - P[i])/ ;

	// point = ...

	return point;

}

void MyViewer::update_scene ()
{
	// Initializations:

	BezCurve->init();
	BSpline->init();
	CrSpline->init();

	// Access the control polygon:
	GsPolygon& P = _polyed->polygon(0);
	float deltat = _slider->value();
	if (_viewA->value()) // show curve
	{
		// Add your curves below and select the correct one(s) to be displayed.
		// As an example, below we have a linear interpolation between endpoints:
		BezCurve->begin_polyline();
		for (float t = 0; t < 1.0f; t += deltat) // note: the t range may change according to the curve
		{
			BezCurve->push(eval_bezier(t, P));
		}
		BezCurve->push(P.top()); // ensure final point is there
		BezCurve->end_polyline();
	}

		if (_viewC->value()) {
			BSpline->begin_polyline();
			for (float t = 2.0f; t < P.size(); t += deltat) {
				BSpline->push(eval_bspline(t, P));
			}
			BSpline->push(P.top()); // ensure final point is there
			BSpline->end_polyline();
		}

		if (_viewD->value()) {
			CrSpline->begin_polyline();
			for (float t = 0.0f; t < P.size()-3; t += deltat) {
				CrSpline->push(crspline (t, P));
			}
			CrSpline->push(P.top()); // ensure final point is there
			CrSpline->end_polyline();
		}
		
	
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{
		case EvViewCurveD:update_scene(); return 1;
		case EvViewCurveA:update_scene(); return 1;
		case EvViewCurveC:update_scene(); return 1;
		case EvViewCurveB:	update_scene(); return 1;
		case EvDeltaT:		update_scene(); return 1;
		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
