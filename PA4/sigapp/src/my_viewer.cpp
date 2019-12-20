
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>
#include <cmath>
# include <iostream>
# include <sigogl/ws_run.h>

static float radius = 2.0f;
static float R = 8.0f;
static float height = 10.0f;
static float width = 5.0f;
static int n = 10;
GsModel* mymodel;// = new GsModel;

//using namespace std;
MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	_nbut=0;
	_animating=false;
	build_ui ();
	build_scene ();
}

void MyViewer::build_ui ()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp;
		p->add ( _nbut=new UiCheckButton ( "Normals", EvNormals ) ); 
	}
	p->add ( new UiButton ( "Animate", EvAnimate ) );
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}

void MyViewer::add_model ( SnShape* s, GsVec p )
{
	// This method demonstrates how to add some elements to our scene graph: lines,
	// and a shape, and all in a group under a SnManipulator.
	// Therefore we are also demonstrating the use of a manipulator to allow the user to
	// change the position of the object with the mouse. If you do not need mouse interaction,
	// you can just use a SnTransform to apply a transformation instead of a SnManipulator.
	// You would then add the transform as 1st element of the group, and set g->separator(true).
	// Your scene graph should always be carefully designed according to your application needs.

	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation ( p );
	manip->initial_mat ( m );

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);
	// manip->visible(false); // call this to turn off mouse interaction

	rootg()->add(manip);
}

void MyViewer::build_scene()
{
	rootg()->init();
	mymodel = new GsModel;
	double centerp = 0;
	double centert = 0;
	double phi = 0;
	double theta = 0;
	double delta = 0;
	double x, y, z = 0;
	double x_2, y_2, z_2 = 0.0;
	double x_3, y_3, z_3 = 0.0;
	double x_4, y_4, z_4 = 0.0;

	SnLines* line = new SnLines;
	line->init();
	GsModel& torus = *mymodel;
	line->color(GsColor::red);
	//display faces
	

	int i0 = 0;		//face index
	line->init();
	int faceIndex = 0;
	int vIndex = 0;
	for (int i = 0; i <= n; i++) {
		phi += (GS_2PI / n);
		for (int j = 0; j <= n; j++) {
			theta += (GS_2PI / n);
			delta = (GS_2PI / n);

			//first point (theta, phi) 0 
			x = (R + radius * cos(theta)) * cos(phi);
			y = ((R + radius * cos(theta)) * sin(phi));
			z = (radius * sin(theta));
			//GS_2PI;

			//second point (theta + delta_theta, phi) 1 
			x_2 = (R + radius * cos(theta + delta)) * cos(phi);
			y_2 = ((R + radius * cos(theta + delta)) * sin(phi));
			z_2 = (radius * sin(theta + delta));

			//third point (theta + delta_theta, phi+delta_phi) 2
			x_3 = (R + radius * cos(theta + delta)) * cos(phi + delta);
			y_3 = ((R + radius * cos(theta + delta)) * sin(phi + delta));
			z_3 = (radius * sin(theta + delta));

			//fourth point (theta, phi+delta_phi) 3
			x_4 = (R + radius * cos(theta)) * cos(phi + delta);
			y_4 = ((R + radius * cos(theta)) * sin(phi + delta));
			z_4 = (radius * sin(theta));

			a = GsVec(x, y, z);
			b = GsVec(x_2, y_2, z_2);
			c = GsVec(x_3, y_3, z_3);
			d = GsVec(x_4, y_4, z_4);

			mymodel->V.push(d);			//0
			mymodel->V.push(c);			//1
			mymodel->V.push(b);			//2
			mymodel->V.push(d);
			mymodel->V.push(b);
			mymodel->V.push(a);			//3

			mymodel->F.push() = GsModel::Face(i0, i0 + 1, i0 + 2);			//d,c,b
			mymodel->F.push() = GsModel::Face(i0+3, i0 + 4, i0 + 5);			//d,b,a
			i0 += 6;

			if (flat) {
				//computes from surface 
				const GsVec& a1 = torus.V[torus.F[faceIndex].a];
				const GsVec& b1 = torus.V[torus.F[faceIndex].b];
				const GsVec& c1 = torus.V[torus.F[faceIndex].c];
				faceIndex++;
		
				GsPnt n_flat1 = cross(b1 - a1, c1-a1);
				mymodel->N.push(n_flat1);

				const GsVec& a2 = torus.V[torus.F[faceIndex].a];
				const GsVec& b2 = torus.V[torus.F[faceIndex].b];
				const GsVec& c2 = torus.V[torus.F[faceIndex].c];
				faceIndex++;
				
				GsPnt n_flat2 = cross(b2 - a2, c2-a2);
				mymodel->N.push(n_flat2);
			}
			else {
				//computes from vertices 
				float one = float(R * cos(phi )), two = float(R * sin(phi )), three = 0;
				float two_1 = float(R * cos(phi +delta)), two_2 = float(R * sin((phi + delta))), two_3 = 0;
				GsPnt centersmooth1 = GsVec(one, two, three);
				GsPnt centersmooth2 = GsVec(two_1, two_2, two_3);

				GsPnt n_a = a - centersmooth1;
				GsPnt n_b = b - centersmooth1;
				GsPnt n_c = c - centersmooth2;
				GsPnt n_d = d - centersmooth2;

				torus.N.push(n_d);
				torus.N.push(n_c);
				torus.N.push(n_b);

				torus.N.push(n_d);
				torus.N.push(n_b);
				torus.N.push(n_a);

				GsPnt n_smooth1 = cross(a - b, c - d);
				GsPnt n_smooth2 = cross(a - c, c - b);

				torus.set_mode(GsModel::Smooth, GsModel::NoMtl);
			}
		
		}
		
	}
	SnModel* myscene = new SnModel(mymodel);
	rootg()->add(myscene);
}

void MyViewer::compute_segments(bool shownormals) {
	SnLines* line = new SnLines;
	line->init();
	GsModel& torus = *mymodel;
	line->color(GsColor::red);
	//double center1, center2;

	if (flat) { 
		//flat normal - normals project from the surface
		for (int i = 0; i < torus.F.size(); i++) {
			const GsVec& a1 = torus.V[torus.F[i].a];
			const GsVec& b1 = torus.V[torus.F[i].b];
			const GsVec& c1 = torus.V[torus.F[i].c];

			GsPnt center = (a1 + b1 + c1 ) / 3.0f;
			
			line->push(center, center + 2.0f * normalize(torus.N[i]));
		}
	}
	
	else {
		//smooth normal - normals project from the vertices]

		for (int j = 0; j < torus.V.size(); j++) {
		//for (int j = 0; j < 2; j++) {
			const GsVec& a2 = torus.V[j];
			line->push(a2, a2 + 2.0f*normalize(torus.N[j]));
		}
		SnModel* flat_normals = new SnModel(mymodel);
		rootg()->add(flat_normals);
	}

	SnModel* myscene = new SnModel(mymodel);
	rootg()->add(line);
}


// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation ()
{
	if ( _animating ) return; // avoid recursive calls
	_animating = true;
	
	int ind = gs_random ( 0, rootg()->size()-1 ); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();

	double frdt = 1.0/30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t=0, lt=0, t0=gs_time();
	do // run for a while:
	{	while ( t-lt<frdt ) { ws_check(); t=gs_time()-t0; } // wait until it is time for next frame
		double yinc = (t-lt)*v;
		if ( t>2 ) yinc=-yinc; // after 2 secs: go down
		lt = t;
		m.e24 += (float)yinc;
		if ( m.e24<0 ) m.e24=0; // make sure it does not go below 0
		manip->initial_mat ( m );
		render(); // notify it needs redraw
		ws_check(); // redraw now
	}	while ( m.e24>0 );
	_animating = false;
}

void MyViewer::show_normals ( bool view )
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for ( int k=0; k<r->size(); k++ )
	{	SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if ( !view ) { l->visible(false); continue; }
		l->visible ( true );
		if ( !l->empty() ) continue; // build only once
		l->init();
		if ( s->instance_name()==SnPrimitive::class_name )
		{	GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face ( fn );
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for ( int i=0; i<m.F.size(); i++ )
			{	const GsVec& a=m.V[m.F[i].a]; l->push ( a, a+(*n++)*f );
				const GsVec& b=m.V[m.F[i].b]; l->push ( b, b+(*n++)*f );
				const GsVec& c=m.V[m.F[i].c]; l->push ( c, c+(*n++)*f );
			}
		}  
	}
}

int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;

	switch ( e.key )
	{	case GsEvent::KeyEsc : gs_exit(); return 1;
	case 'n': { bool b = !_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
	case 'q': {//increment number of faces
		//destroy current torus and redraw
		n+=1;
		//rootg()->remove_all();
		build_scene();
		return 1; 
	}
	case 'a': {//decrement number of faces
		n -= 1;
		rootg()->remove_all();
		build_scene();
		return 1;
	}
	case 'w': {
		//increment radius
		radius += 1;
		rootg()->remove_all();
		build_scene();
		return 1;

	}
	case 's': {
		//decrement radius
		radius -= 1;
		rootg()->remove_all();
		build_scene();
		return 1;
	}
	case 'e': {
		//increment big radius
		R += 1;
		rootg()->remove_all();
		build_scene();
		return 1;
	}
	case 'd': {
		// decrement big radius
		R -= 1;
		rootg()->remove_all();
		build_scene();
		return 1;
	}
	case 'z': {
		//enable flat shading 
		flat = true;
		rootg()->remove_all();

		build_scene();
		//compute_segments(flat);
		return 1;
	}
	case 'x': {
		//enable smooth shading
		flat = false;
		rootg()->remove_all();
		build_scene();
		//compute_segments(flat);
		return 1;
	}
	case 'c': {
		//show normal vectors
		shownormals = true;
		build_scene();
		compute_segments(shownormals);
		return 1;
	}
	case 'v': {
		//dont show normal vectors
		
		rootg()->remove_all();
		build_scene();
		return 1;
	}
			//default: gsout<<"Key pressed: "<<e.key<<gsnl;
	
	}

	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	case EvNormals: show_normals(_nbut->value()); return 1;
		case EvAnimate: run_animation(); return 1;
		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
