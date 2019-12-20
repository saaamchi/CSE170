
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>
#include <cmath>
# include <iostream>
# include <sigogl/ws_run.h>

GsModel* mymodel;// = new GsModel
static float radius = 0.12f;
static float R = 0.3f;
static float height = 0.3f;
static float width = 0.3f;
static int n = 30;
SnManipulator* top = new SnManipulator;
SnManipulator* bottom = new SnManipulator;
SnManipulator* frontwheel1 = new SnManipulator;
SnManipulator* frontwheel2= new SnManipulator;
SnManipulator* backwheel1 = new SnManipulator;
SnManipulator* backwheel2 = new SnManipulator;
SnManipulator* cardoor1 = new SnManipulator;
SnManipulator* cardoor2 = new SnManipulator;
SnManipulator* ttrunk = new SnManipulator;
SnManipulator* btrunk = new SnManipulator;


//using namespace std;
MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	_nbut=0;
	_animating=false;
	build_ui ();
	build_scene ();
	on = false; 
	xpos = 0.0f; 
	ypos = 0.0f;
	car_rot = 0.0f;
	door_rot = 0.0f;
	door_rot2 = 0.0f;
	fwheel = 0.0f;
	bwheel = 0.0f;
	trunkrot = 0.0f;
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
	SnGroup* pos_sng = new SnGroup;

	//top part of car
	SnPrimitive* tcar_sn =new SnPrimitive(GsPrimitive::Box, 1.2f, 0.7f, 1.49f);
	tcar_sn->prim().material.diffuse = GsColor::magenta;
	transm.setrans(-0.1f, 1.0f, 0);
	
	SnGroup* topgroup = new SnGroup;
	topgroup->add(tcar_sn);
	top->child(topgroup);
	pos_sng->add(top);
	top->initial_mat(transm);
	rootg()->add(top);

	//body of car
	SnPrimitive* bcar_sn = new SnPrimitive(GsPrimitive::Box, 2.0f, 0.5f, 1.5f);
	bcar_sn->prim().material.diffuse = GsColor::blue;
	transm.setrans(0.0f, -1.0f, 0.0f);

	SnGroup* bottomsng = new SnGroup;
	bottomsng->add(bcar_sn);
	bottom->child(bottomsng);
	topgroup->add(bottom);
	bottom->initial_mat(transm);

	//car doors 
	SnPrimitive* frontdoor1 = new SnPrimitive(GsPrimitive::Box, 0.8f, 0.3f, 0.07f);
	frontdoor1->prim().material.diffuse = GsColor::cyan;
	transm.setrans(0.0f, 0.0f, 1.5f);

	SnGroup* doorgroup1 = new SnGroup;
	doorgroup1->add(frontdoor1);
	cardoor1->child(doorgroup1);
	bottomsng->add(cardoor1);
	cardoor1->initial_mat(transm);


	SnPrimitive* frontdoor2 = new SnPrimitive(GsPrimitive::Box, 0.8f, 0.3f, 0.07f);
	frontdoor2->prim().material.diffuse = GsColor::cyan;
	transm.setrans(0.0f, 0.0f, -1.5f);

	SnGroup* doorgroup2 = new SnGroup;
	doorgroup2->add(frontdoor2);
	cardoor2->child(doorgroup2);
	bottomsng->add(cardoor2);
	cardoor2->initial_mat(transm);

	//trunk 
	SnPrimitive* toptrunk = new SnPrimitive(GsPrimitive::Box, 0.7f, 0.05f, 0.7f);
	toptrunk->prim().material.diffuse = GsColor::cyan;
	transm.setrans(-1.35f, 0.5f, 0.0f);

	SnGroup* trunkgroup1 = new SnGroup;
	trunkgroup1->add(toptrunk);
	ttrunk->child(trunkgroup1);
	bottomsng->add(ttrunk);
	ttrunk->initial_mat(transm);

	SnPrimitive* bottomtrunk = new SnPrimitive(GsPrimitive::Box, 0.05f, 0.5f, 0.8f);
	bottomtrunk->prim().material.diffuse = GsColor::cyan;
	transm.setrans(-0.6f, -0.45f, 0.0f);

	SnGroup* trunkgroup2 = new SnGroup;
	trunkgroup2->add(bottomtrunk);
	btrunk->child(trunkgroup2);
	trunkgroup1->add(btrunk);
	btrunk->initial_mat(transm);

	//grass floor
	SnPrimitive* p;
	p = new SnPrimitive(GsPrimitive::Box, 6.0f, 0.07f, 8.0f);
	p->prim().material.diffuse = GsColor::darkred;
	add_model(p, GsVec(0.0f, -0.8f, 0.0f));

	//front trees
	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 1.0f, 0.2f);
	p->prim().material.diffuse = GsColor::green;
	add_model(p, GsVec(5.0f, 2.8f, 7.0f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.6f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(5.0f, 0.9f, 7.0f));


	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 0.8f, 0.2f);
	p->prim().material.diffuse = GsColor::darkgreen;
	add_model(p, GsVec(4.1f, 2.5f, 6.4f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.4f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(4.1f, 0.7f, 6.4f));

	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 0.8f, 0.2f);
	p->prim().material.diffuse = GsColor::green;
	add_model(p, GsVec(3.1f, 2.7f, 7.0f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.6f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(3.1f, 0.9f, 7.0f));

	//back trees 
	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 1.0f, 0.2f);
	p->prim().material.diffuse = GsColor::darkgreen;
	add_model(p, GsVec(-5.0f, 2.8f, -7.0f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.6f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(-5.0f, 0.9f, -7.0f));

	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 0.8f, 0.2f);
	p->prim().material.diffuse = GsColor::green;
	add_model(p, GsVec(-4.1f, 2.5f, -6.4f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.4f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(-4.1f, 0.7f, -6.4f));

	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 0.8f, 0.2f);
	p->prim().material.diffuse = GsColor::darkgreen;
	add_model(p, GsVec(-3.1f, 2.7f, -7.0f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.6f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(-3.1f, 0.9f, -7.0f));

	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 0.8f, 0.2f);
	p->prim().material.diffuse = GsColor::green;
	add_model(p, GsVec(-3.1f, 2.5f, -6.4f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.4f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(-3.1f, 0.7f, -6.4f));

	p = new SnPrimitive(GsPrimitive::Capsule, 0.2f, 0.8f, 0.2f);
	p->prim().material.diffuse = GsColor::darkgreen;
	add_model(p, GsVec(-2.1f, 2.7f, -7.0f));
	p = new SnPrimitive(GsPrimitive::Cylinder, 0.1f, 0.1f, 1.6f);
	p->prim().material.diffuse = GsColor::brown;
	add_model(p, GsVec(-2.1f, 0.9f, -7.0f));

	//front wheel 
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
	
	SnModel* frontwheel1_sn = new SnModel(&torus);

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
			mymodel->F.push() = GsModel::Face(i0 + 3, i0 + 4, i0 + 5);			//d,b,a
			i0 += 6;

			float one = float(R * cos(phi)), two = float(R * sin(phi)), three = 0;
			float two_1 = float(R * cos(phi + delta)), two_2 = float(R * sin((phi + delta))), two_3 = 0;
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

		}
	}

	GsModel::Group& g = *torus.G.push();
	//fi is the intial face 
	//fn is the number of faces
	g.fi = 0;
	g.fn = torus.F.size();
	g.dmap = new GsModel::Texture;
	
	g.dmap->fname.set("water.png");
	torus.M.push().init();					//M - list of materials 
	torus.M.top() = GsMaterial::DefaultSpecular;
	int nv = torus.V.size();				// V - list of vertex coordinates 
	torus.T.size(nv);						//T - list of texture coordinates

	phi = 0.0;
	theta = 0.0;
	int k = 0;

	for (int i = 0; i <= n; i++) {
		phi += (GS_2PI / n);
		double delta_phi = (GS_2PI / n);
		for (int j = 0; j <= n; j++) {
			double delta_theta = (GS_2PI / n);
			theta += (GS_2PI / n);
			torus.T[k].set((theta / GS_2PI), (phi + delta_phi) / GS_2PI);						//d
			torus.T[k + 1].set((theta + delta_theta) / GS_2PI, (phi + delta_phi) / GS_2PI);	//c
			torus.T[k + 2].set((theta + delta_theta) / GS_2PI, phi / GS_2PI);				//b
			torus.T[k + 3].set(theta / GS_2PI, (phi + delta_phi) / GS_2PI);					//d
			torus.T[k + 4].set((theta + delta_theta) / GS_2PI, phi / GS_2PI);				//b
			torus.T[k + 5].set(theta / GS_2PI, phi / GS_2PI);								//a
			k += 6;

		}
	}
	torus.textured = true;
	torus.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	//rootg()->add(myscene);

	transm.setrans(1.4f, -0.4f, 1.5f);
	SnGroup* fwheelgroup = new SnGroup;
	fwheelgroup->add(frontwheel1_sn);
	frontwheel1->child(fwheelgroup);
	bottomsng->add(frontwheel1);
	frontwheel1->initial_mat(transm);
	
	GsModel& torus2 = *mymodel;
	SnModel* frontwheel2_sn = new SnModel(&torus2);
	transm.setrans(1.4f, -0.4f, -1.5f);

	SnGroup* fwheelgroup2 = new SnGroup;
	fwheelgroup2->add(frontwheel2_sn);
	frontwheel2->child(fwheelgroup2);
	bottomsng->add(frontwheel2);
	frontwheel2->initial_mat(transm);

	//back wheels 
	GsModel& torus3 = *mymodel;
	SnModel* backwheel1_sn = new SnModel(&torus3);
	transm.setrans(-1.4f, -0.4f, 1.5f);

	SnGroup* backwheelgroup1 = new SnGroup;
	backwheelgroup1->add(backwheel1_sn);
	backwheel1->child(backwheelgroup1);
	bottomsng->add(backwheel1);
	backwheel1->initial_mat(transm);

	GsModel& torus4 = *mymodel;
	SnModel* backwheel2_sn = new SnModel(&torus4);
	transm.setrans(-1.4f, -0.4f, -1.5f);

	SnGroup* backwheelgroup2 = new SnGroup;
	backwheelgroup2->add(backwheel2_sn);
	backwheel2->child(backwheelgroup2);
	bottomsng->add(backwheel2);
	backwheel2->initial_mat(transm);

}

void MyViewer::camera1(bool on) {
	//GsCamera* c = new GsCamera;
	
	if (on) {
		double lt = 0.0; 
		double t0 = gs_time();
		do {
			lt = gs_time() - t0;
			camera().eye.y += 0.02f;

			//look in general distance but look away
			camera().center.x += 0.01f;

			//tilt head left
			//camera().up.x += 0.01f;
			
			render();
			ws_check();
			message().setf("local time =%f", lt);
		} while (lt < 3.0f);

	//	on = false;
	}
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

	switch (e.key)
	{
	case GsEvent::KeyEsc: gs_exit(); return 1;
	case 'n': { bool b = !_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
	case 'q': { //increment car rotation
		car_rot += (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = top->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(car_rot);
		final = tm * rot * origin;
		top->initial_mat(final);
		render();
		return 1;
	}
	case 'a': { //decrement car rotation
		car_rot -= (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = top->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(car_rot);
		final = tm * rot * origin;
		top->initial_mat(final);
		render();
		return 1;
	}
	case 'w': {
		//open front door 
		door_rot += (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		///*! Get the translation fields of the matrix and put in tx, ty, tz */
		//void getrans(float& tx, float& ty, float& tz) const { tx = e[3]; ty = e[7]; tz = e[11]; }
		
		mat = cardoor1->mat();
		currentpoint = { mat.e14+0.03f,mat.e24,mat.e34+0.02f };
		tm.translation(currentpoint);

		rot.roty(door_rot);
		final = tm * rot * origin;
		cardoor1->initial_mat(final);
		render();

		return 1;
	}
	case 's': {
		//close front door 
		door_rot -= (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = cardoor1->mat();
		currentpoint = { mat.e14- 0.035f,mat.e24,mat.e34-0.02f };
		tm.translation(currentpoint);

		rot.roty(door_rot);
		final = tm * rot * origin;
		cardoor1->initial_mat(final);
		render();

		return 1;
	}
	case 'e': {
		//open back door
		door_rot2 -= (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = cardoor2->mat();
		currentpoint = { mat.e14+ 0.035f,mat.e24,mat.e34 -0.02f };
		tm.translation(currentpoint);

		rot.roty(door_rot2);
		final = tm * rot * origin;
		cardoor2->initial_mat(final);
		render();

		return 1;
	}
	case 'd': {
		//close back door
		door_rot2 += (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = cardoor2->mat();
		currentpoint = { mat.e14- 0.035f,mat.e24,mat.e34 + 0.02f };
		tm.translation(currentpoint);

		rot.roty(door_rot2);
		final = tm * rot * origin;
		cardoor2->initial_mat(final);
		render();

		return 1;
	}
	case 'r': { //open car trunk
		trunkrot -= (float)(GS_2PI / 90);
		//gsout << car_rot;
		mat = ttrunk->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		origin.translation(0,0,0);				
		tm.translation(currentpoint);

		rot.rotz(trunkrot);
		final = tm * rot;// *origin;
		ttrunk->initial_mat(final);
		render();
		return 1;
	}
	case 'f': { //close car trunk 
		trunkrot += (float)(GS_2PI / 90);
		//gsout << car_rot;
		origin.translation(0, 0, 0);
		mat = ttrunk->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.rotz(trunkrot);
		final = tm * rot * origin;
		ttrunk->initial_mat(final);
		render();
		return 1;
	}
	case 't': {
		//clockwise front wheel
		fwheel += (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = frontwheel1->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(fwheel);
		final = tm * rot * origin;
		frontwheel1->initial_mat(final);

		origin.translation(0, 0, 0);
		mat = frontwheel2->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(fwheel);
		final = tm * rot * origin;
		frontwheel2->initial_mat(final);
		render();
		return 1;
	}
	case 'g': {
		//counter clockwise front wheel 
		fwheel -= (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = frontwheel1->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(fwheel);
		final = tm * rot * origin;
		frontwheel1->initial_mat(final);

		origin.translation(0, 0, 0);
		mat = frontwheel2->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(fwheel);
		final = tm * rot * origin;
		frontwheel2->initial_mat(final);
		render();
		return 1;
	}
	case 'y': {
		//clockwise back wheel 
		bwheel += (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = backwheel1->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(bwheel);
		final = tm * rot * origin;
		backwheel1->initial_mat(final);

		origin.translation(0, 0, 0);
		mat = backwheel2->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(bwheel);
		final = tm * rot * origin;
		backwheel2->initial_mat(final);
		render();
		return 1;
	}
	case 'h': {
		//counter clock wise back wheel  
		bwheel -= (float)(GS_2PI / 90);
		origin.translation(0, 0, 0);
		mat = backwheel1->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(bwheel);
		final = tm * rot * origin;
		backwheel1->initial_mat(final);

		origin.translation(0, 0, 0);
		mat = backwheel2->mat();
		currentpoint = { mat.e14,mat.e24,mat.e34 };
		tm.translation(currentpoint);

		rot.roty(bwheel);
		final = tm * rot * origin;
		backwheel2->initial_mat(final);
		render();
		return 1;
	}

//global movements 
	case GsEvent::KeyUp:{
		cartrans = top->mat();
		ypos += 0.3f;
		cartrans.e24 = ypos;
		top->initial_mat(cartrans);
		render();
		return 1;
	 }
	case GsEvent::KeyDown: {
		cartrans = top->mat();
		ypos -= 0.3f;
		cartrans.e24 = ypos;
		top->initial_mat(cartrans);
		render();
		return 1;
	}
	case GsEvent::KeyRight: {
		cartrans = top->mat();
		xpos += 0.3f;
		cartrans.e14 = xpos;
		top->initial_mat(cartrans);
		render();
		return 1;
	}
	case GsEvent::KeyLeft: {
		cartrans = top->mat();
		xpos -= 0.3f;
		cartrans.e14 = xpos;
		top->initial_mat(cartrans);
		render();
		return 1;
	}
	case GsEvent::KeySpace: {
		on = true;
		camera1(on);
		return 1;
	}
	
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
