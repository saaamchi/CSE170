
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>
# include "../vs2019/glr_mynode.h"
#include "../vs2019/sn_mynode.h"
#include <cmath>


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
void MyViewer::add_mynode(int n)
{
	//SnMyNode* c = new SnMyNode;
	SnMyNode* c;

	float r = 0.15f; // position range
	while (n-- > 0) {
		c = new SnMyNode;


		c->init.set(0,0,0);
		c->width = gs_random(0.001f, r);
		c->height = gs_random(0.001f, r * 2);

		// Example how to print/debug your generated data:
		// gsout<<n<<": "<<c->color()<<gsnl;
		//rootg()->add(c);
		//}
	}
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

void MyViewer::build_scene ()
{
	SnPrimitive* p;

	SnMyNode* torus = new SnMyNode;

	//shape, color, position
	//clock
	add_mynode(1);
	rootg()->add(torus);

	//big hand
	p = new SnPrimitive(GsPrimitive::Capsule, 0.5f, 0.5f, 6);
	p->prim().material.diffuse = GsColor::magenta;
	add_model(p, GsVec(0, 6, 0));

	//short hand
	p = new SnPrimitive(GsPrimitive::Capsule, 0.5f, 0.5f, 3);
	p->prim().material.diffuse = GsColor::lightblue;
	add_model(p, GsVec(1, 3, 0));
}

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation ()
{
	longhand_angle = 0.0f;
	shorthand_angle = 0.0f;
	if (_animating) return; // avoid recursive calls

	_animating = true;
	
	//gsout << rootg()->size() << gsnl;
	//int ind = gs_random ( 0, rootg()->size()-1 ); // pick one child
	//gsout << ind << gsnl;

	//int hand = 1; // starts animation w the seconds hand
	SnManipulator* shorthand = rootg()->get<SnManipulator>(1); // access one of the manipulators
	GsMat rot_short = shorthand->mat();
	SnManipulator* longhand = rootg()->get<SnManipulator>(2); // access one of the manipulators
	GsMat rot_long = longhand->mat();

	double frdt = 1.0/60.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double current_time = 0, last_time = current_time, t0 = gs_time();
	
	float trans_x = 0.0f;
	float trans_y = 0.0f;

	do
	{
		if (_animating ){
			//increment the rotation 
			longhand_angle += (float)(-GS_2PI / 360);				//slower
			shorthand_angle += (float)(-GS_2PI / 60);				//faster
			flag = 1;
		}
		else {
			flag = 0;
		}
		
		while (current_time - last_time < frdt) {
			ws_check();
			current_time = gs_time() - t0;
		}
		last_time = current_time;

		//long hand translation equation
		/*trans_x = 3 * sin(longhand_angle);
		trans_y = (3 * cos(longhand_angle)) +2 ;*/
		

		//rot_long.translation(-trans_y,-trans_x, 0);
		rot_long.rotz(longhand_angle);
		//rot_long.translation(trans_y, trans_x, 0);
		longhand->initial_mat(rot_long);


		trans_x = 3 * sin(shorthand_angle);
		trans_y = (3 * cos(shorthand_angle)) + 2;

		//rot_short.translation(-trans_y, -trans_x, 0);
		rot_short.rotz(shorthand_angle);
		//rot_short.translation(trans_y, trans_x, 0);

		shorthand->initial_mat(rot_short);


		render(); // notify it needs redraw
		ws_check(); // redraw now

	}
	while (flag == 1);
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
			const GsVec* n = fn;
			fn.pt();
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
	MyViewer* k = (MyViewer*)rootg()->get(0);
	
	switch ( e.key )
	{	case GsEvent::KeyEsc : gs_exit(); return 1;
		case 'n' : { bool b=!_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
		default: gsout<<"Key pressed: "<<e.key<<gsnl;
		case GsEvent::KeyEnter: _animating = false; longhand_angle = 0.0f; shorthand_angle = 0.0f;  return 1; //should be okay
		case GsEvent::KeySpace: flag = 0; return 1;
			
		
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
