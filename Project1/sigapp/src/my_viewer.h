# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	bool _animating;
   public :
	   bool on;
	   float xpos;
	   float ypos;
	   float car_rot;
	   float door_rot, door_rot2;
	   float fwheel;
	   float bwheel;
	   float trunkrot;
	   GsPnt a, b, c, d;
	   GsMat origin, cartrans, transm, mat, tm, rot,final,currentmatrix;
	   GsVec currentpoint;
	
	  
	MyViewer ( int x, int y, int w, int h, const char* l );
	void build_ui ();
	void add_model ( SnShape* s, GsVec p );
	void build_scene ();
	void camera1(bool on);
	//void compute_segments(bool flat);
	void show_normals ( bool view );
	void run_animation ();
	
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
};

