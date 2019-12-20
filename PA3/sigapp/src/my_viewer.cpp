
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>
# include "cmath"

SnManipulator* upperarm_mani = new SnManipulator;
SnManipulator* lowerarm_mani = new SnManipulator;
SnManipulator* hand_mani = new SnManipulator;

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

void MyViewer::build_scene ()
{
	//rotating transformation
	GsMat f, m;
	GsModel* rot_gs = new GsModel();
	SnModel* rot_sn;
	rot_sn = new SnModel(rot_gs);

	SnManipulator* rotationmanip = new SnManipulator;
	SnGroup* rot_sng = new SnGroup;
	rot_sng->add(rot_sn);
	rotationmanip->child(rot_sng);
	f.roty((float)GS_2PI/4);
	transm.setrans(0.0f, 0.0f, 0.0f);
	rotationmanip->initial_mat(f*transm);
	rootg()->add(rotationmanip);

	//upperarm
	GsModel* upperarm_gs = new GsModel();
	SnModel* upperarm_sn;
	upperarm_gs->load("../../arm/rupperarm.m");
	upperarm_sn = new SnModel(upperarm_gs);

	SnGroup* upperarm_sng = new SnGroup;
	upperarm_sng->add(upperarm_sn);
	upperarm_mani->child(upperarm_sng);
	rot_sng->add(upperarm_mani);
	//rootg()->add(upperarm_mani);

	
	transm.setrans(0.0f, 0.0f, 0.0f);
	upperarm_mani->initial_mat(transm);

	//lowerarm
	GsModel* lowerarm_gs = new GsModel();
	SnModel* lowerarm_sn;
	lowerarm_gs->load("../../arm/rlowerarm.m");
	lowerarm_sn = new SnModel(lowerarm_gs);

	SnGroup* lowerarm_sng = new SnGroup;
	//SnManipulator* lowerarm_mani = new SnManipulator;
	lowerarm_sng->add(lowerarm_sn);
	lowerarm_mani->child(lowerarm_sng);
	upperarm_sng->add(lowerarm_mani);

	transm.setrans(0.0f, 0.0f, 27.0f);
	lowerarm_mani->initial_mat(transm);

	//hand
	GsModel* hand_gs = new GsModel();
	SnModel* hand_sn;
	hand_gs->load("../../arm/rhand.m");
	hand_sn = new SnModel(hand_gs);

	SnGroup* hand_sng = new SnGroup;
	//SnManipulator* hand_mani = new SnManipulator;
	hand_sng->add(hand_sn);
	hand_mani->child(hand_sng);
	lowerarm_sng->add(hand_mani);
	//rootg()->add(hand_mani);
	
	//z,y,x
	transm.setrans(0.0f, 0.0f, 25.0f);
	hand_mani->initial_mat(transm);

	//first rot matrix will put it on the correct axis- roty

	//matrix transform
	//first model 

	//matrix transform 
	//second model

	//matrix transform
	//thirdmodel 
}

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation ()
{
	if ( _animating ) return; // avoid recursive calls
	_animating = true;
	
	int ind = gs_random ( 0, rootg()->size()-1 ); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();

	double frdt = 1.0/5.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t=0, lt=0, t0=gs_time();

	shoulderangle = 0.0f;
	/*shoulderangle += (float)(GS_2PI / 90);
	elbowangle += (float)(GS_2PI / 90);
	wristangle += (float)(GS_2PI / 90);*/

	//float delta_angle_1 = a1 - a2;
	//float ange_step = 1.5f;
	//int steps1 = delta_angle_1 / ange_step;

	do // run for a while:
	{	while ( t-lt<frdt ) { ws_check(); t=gs_time()-t0; } // wait until it is time for next frame
		double yinc = (t-lt)*v;
		//while ( t<2 ) shoulderangle=+shoulderangle; // after 2 secs: go down
		///example

		while (shoulderangle <= 180.0f)

		{
			shoulderangle += 1.0f;
			origin.translation(0, 0, 0);				//origin matrix
			mat = upperarm_mani->mat();					//get current matrix
			currentpoint = { mat.e14,mat.e24,mat.e34 }; // save current translation points 
			tm.translation(currentpoint);				//make a new translation matrix with those points

			rot.rotx(shoulderangle);					//angle matrix 
			final = tm * rot * origin;
			upperarm_mani->initial_mat(final);

			render(); // notify it needs redraw
			ws_check(); // redraw now
			////roate_upper(angle_step_1);
			//rotate_lower(angle_step_2)

		}
		while (elbowangle <= 45.0f) {
			elbowangle += 1.0f;
			origin.translation(0, 0, 0);
			mat = lowerarm_mani->mat();
			currentpoint = { mat.e14,mat.e24,mat.e34 };
			tm.translation(currentpoint);


			if ((elbowangle - (float)(GS_2PI / 90.0)) < (float)(-GS_PI / 4.0))
				elbowangle = (float)-GS_PI / 4.0;
			else elbowangle -= (float)(GS_2PI / 90.0);
			rot.rotx(elbowangle);


			final = tm * rot * origin;
			lowerarm_mani->initial_mat(final);
			render();
			ws_check();

		}
		while (wristangle <= 45.0f) {
			wristangle+=1.0f;
			origin.translation(0, 0, 0);
			mat = hand_mani->mat();
			currentpoint = { mat.e14,mat.e24,mat.e34 };
			tm.translation(currentpoint);

			if ((wristangle - (float)(GS_2PI / 90.0)) < (float)(-GS_PI / 4.0))
				wristangle = (float)-GS_PI / 4.0;
			else wristangle -= (float)(GS_2PI / 90.0);
			rot.rotx(wristangle);
			final = tm * rot * origin;
			hand_mani->initial_mat(final);
			render();
		}

		///
		//lt = t;
		//m.e24 += (float)yinc;
		//if ( m.e24<0 ) m.e24=0; // make sure it does not go below 0
		//manip->initial_mat ( m );

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
	{	
		case GsEvent::KeyEsc : gs_exit(); return 1;
		case 'n' : { bool b=!_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
		case 'q': {
			
			//increment shoulder angle
			
			shoulderangle += (float)(GS_2PI / 90);
			origin.translation(0, 0, 0);				//origin matrix
			mat = upperarm_mani->mat();					//get current matrix
			currentpoint = { mat.e14,mat.e24,mat.e34 }; // save current translation points 
			tm.translation(currentpoint);				//make a new translation matrix with those points
			
			rot.rotx(shoulderangle);					//angle matrix 
			final = tm * rot * origin;
			upperarm_mani->initial_mat(final);
			render();
			return 1;
		}
		case 'a': {
			//decrement shoulder angle
			shoulderangle -= (float)(GS_2PI / 90);
			origin.translation(0, 0, 0);
			mat = upperarm_mani->mat();
			currentpoint = { mat.e14,mat.e24,mat.e34 };
			tm.translation(currentpoint);

			rot.rotx(shoulderangle);
			final = tm * rot * origin;
			upperarm_mani->initial_mat(final);
			render();
			return 1;
		}
		case 'w': {
			//increment elbow angle 
			
			origin.translation(0, 0, 0);
			mat = lowerarm_mani->mat();
			currentpoint = { mat.e14,mat.e24,mat.e34 };
			tm.translation(currentpoint);


			if ((elbowangle - (float)(GS_2PI / 90.0)) < (float)(-GS_PI / 4.0))
				elbowangle = (float)-GS_PI / 4.0;
			else elbowangle -= (float)(GS_2PI / 90.0);
				rot.rotx(elbowangle);

			
			final = tm * rot * origin;
			lowerarm_mani->initial_mat(final);
			render();
			return 1; 

		}
		case 's': {
			//decrement elbow angle
			if (elbowangle + GS_2PI / 90.0 > 0.0) 
				elbowangle = 0.0; 
			else elbowangle += (float)(GS_2PI / 90.0);
			origin.translation(0, 0, 0);
			mat = lowerarm_mani->mat();
			currentpoint = { mat.e14,mat.e24,mat.e34 };
			tm.translation(currentpoint);

			rot.rotx(elbowangle);
			final = tm * rot * origin;
			lowerarm_mani->initial_mat(final);
			render();
			return 1;
		}
		case 'e': {
			//increment wrist angle 
			//wristangle += (float)(GS_2PI / 90);
			origin.translation(0, 0, 0);
			mat = hand_mani->mat();
			currentpoint = { mat.e14,mat.e24,mat.e34 };
			tm.translation(currentpoint);

			if ((wristangle - (float)(GS_2PI / 90.0)) < (float)(-GS_PI / 4.0))
				wristangle = (float)-GS_PI / 4.0;
			else wristangle -= (float)(GS_2PI / 90.0);
				rot.rotx(wristangle);
			final = tm * rot * origin;
			hand_mani->initial_mat(final);
			render();
			return 1;
		}
		case 'd': {
			//decrement wrist angle
			//wristangle -= (float)(GS_2PI / 90);
			origin.translation(0, 0, 0);
			mat = hand_mani->mat();
			currentpoint = { mat.e14,mat.e24,mat.e34 };
			tm.translation(currentpoint);
			if (wristangle + GS_2PI / 90.0 > 0.0)
				wristangle = 0.0;
			else wristangle += (float)(GS_2PI / 90.0);
			rot.rotx(wristangle);
			final = tm * rot * origin;
			hand_mani->initial_mat(final);
			render();
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
