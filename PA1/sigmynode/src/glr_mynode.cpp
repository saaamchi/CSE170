
# include "sn_mynode.h"
# include "glr_mynode.h"

# include <sig/gs_array.h>
# include <sig/gs_quat.h>

# include <sigogl/gl_core.h>
# include <sigogl/gl_context.h>
# include <sigogl/gl_resources.h>

//# define GS_USE_TRACE1 // Constructor and Destructor
//# define GS_USE_TRACE2 // Render
# include <sig/gs_trace.h>
#include <cmath>

//======================================= GlrLines ====================================

GlrMyNode::GlrMyNode ()
{
	GS_TRACE1 ( "Constructor" );
	_psize = 0;
}

GlrMyNode::~GlrMyNode ()
{
	GS_TRACE1 ( "Destructor" );
}

// These are static because they are the same for all GlrMyNode instances:
static const GlProgram* ProgSC=0; // single color glsl program
static const GlProgram* ProgMC=0; // multi color glsl program

void GlrMyNode::init ( SnShape* s )
{
	GS_TRACE2 ( "Generating program objects" );
	bool multicolor = ((SnMyNode*)s)->multicolor;
	bool LoadMyShaders=true;

	// Initialize program and buffers if needed:
	if ( multicolor && !ProgMC ) // if multiple colors are used, a vertex shader also receiving array of colors is needed
	{	if ( LoadMyShaders ) // Example of how to load specific shader files:
		{	const GlShader* MyVtxShader  = GlResources::declare_shader ( GL_VERTEX_SHADER, "MyVtxShaderMC", "../../sig/shaders/3dsmooth.vert", 0 );
			const GlShader* MyFragShader = GlResources::declare_shader ( GL_FRAGMENT_SHADER, "MyFragShaderMC", "../../sig/shaders/gouraud.frag", 0 );
			const GlProgram* p = GlResources::declare_program ( "MyNodeProgramMC", 2, MyVtxShader, MyFragShader );
			GlResources::declare_uniform ( p, 0, "vProj" );
			GlResources::declare_uniform ( p, 1, "vView" );
			GlResources::compile_program ( p );
			ProgMC = p; // Save in Prog a direct pointer to the program used by this node!
		}
		else // Here we just reuse an internal shader:
		{	ProgMC = GlResources::get_program("3dsmooth"); // has vertex shader receiving arrays of vertices and colors
		}
	}
	else if ( !multicolor && !ProgSC ) // not using multiple colors (just single color)
	{	if ( LoadMyShaders )
		{	const GlShader* MyVtxShader  = GlResources::declare_shader ( GL_VERTEX_SHADER, "MyVtxShaderSC", "../../sig/shaders/3dsmoothsc.vert", 0 );
			const GlShader* MyFragShader = GlResources::declare_shader ( GL_FRAGMENT_SHADER, "MyFragShaderSC", "../../sig/shaders/gouraud.frag", 0 );
			const GlProgram* p = GlResources::declare_program ( "MyNodeProgramSC", 2, MyVtxShader, MyFragShader );
			GlResources::declare_uniform ( p, 0, "vProj" );
			GlResources::declare_uniform ( p, 1, "vView" );
			GlResources::declare_uniform ( p, 2, "vColor" );
			GlResources::compile_program ( p );
			ProgSC = p; // Save in Prog a direct pointer to the program used by this node!
		}
		else // Here we just reuse an internal shader:
		{	ProgSC = GlResources::get_program("3dsmoothsc"); // has vertex shader receiving array of vertices and single color as uniform
		}
	}

	_glo.gen_vertex_arrays ( 1 );
	_glo.gen_buffers ( multicolor? 2:1 );
}

void GlrMyNode::render ( SnShape* s, GlContext* ctx )
{
	GS_TRACE2 ( "GL4 Render "<<s->instance_name() );
	SnMyNode& c = *((SnMyNode*)s);
	bool multicolor = ((SnMyNode*)s)->multicolor;

	// 1. Set buffer data if node has been changed:
	if (s->changed() & SnShape::Changed) // flags are: Unchanged, RenderModeChanged, MaterialChanged, Changed
	{
		/*GsPnt o = c.init;
		float w = c.width;
		float h = c.height;*/
		float radius = c.radius;
		float R = c.R;
		int n = c.n;
		double x, y, z = 0;
		double x_2, y_2, z_2 = 0.0;
		double x_3, y_3, z_3 = 0.0;
		double x_4, y_4, z_4 = 0.0;

		double phi = 0;
		double delta = 0;
		double theta = 0;
		if (R <= radius || n <= 3) return; // invalid parameters

		GsArray<GsVec> P(0, n * n * 6); // will hold the points forming my triangles (size 0, but pre-allocate 6 spaces)

		/*P.push() = o;
		P.push() = o+GsVec(w,0,0);
		P.push() = o+GsVec(w,h,0);
		P.push() = o;
		P.push() = o+GsVec(w,h,0);
		P.push() = o+GsVec(0,h,0);
		*/
		//gsout << (360/10)*3.1415f/180 << gsnl;

		for (int i = 0; i <= n; i++) {

			phi += (GS_2PI / n);

			for (int j = 0; j <= n; j++) {
				theta += (GS_2PI / n);
				delta = (GS_2PI / n);

				//first point (theta, phi)
				x = (R + radius * cos(theta)) * cos(phi);
				y = ((R + radius * cos(theta)) * sin(phi));
				z = (radius * sin(theta));
				//GS_2PI;

				//second point (theta + delta_theta, phi)
				x_2 = (R + radius * cos(theta + delta)) * cos(phi);
				y_2 = ((R + radius * cos(theta + delta)) * sin(phi));
				z_2 = (radius * sin(theta + delta));

				//third point (theta + delta_theta, phi+delta_phi)
				x_3 = (R + radius * cos(theta + delta)) * cos(phi + delta);
				y_3 = ((R + radius * cos(theta + delta)) * sin(phi + delta));
				z_3 = (radius * sin(theta + delta));


				//fourth point (theta, phi+delta_phi)
				x_4 = (R + radius * cos(theta)) * cos(phi + delta);
				y_4 = ((R + radius * cos(theta)) * sin(phi + delta));
				z_4 = (radius * sin(theta));
				
				P.push(GsVec(x, y, z));
				P.push(GsVec(x_2, y_2, z_2));
				P.push(GsVec(x_3, y_3, z_3));

				P.push(GsVec(x, y, z));
				P.push(GsVec(x_3, y_3, z_3));
				P.push(GsVec(x_4, y_4, z_4));
				
			}
		}

		glBindVertexArray ( _glo.va[0] );
		glEnableVertexAttribArray ( 0 );
		glBindBuffer ( GL_ARRAY_BUFFER, _glo.buf[0] );
		glBufferData ( GL_ARRAY_BUFFER, P.sizeofarray(), P.pt(), GL_STATIC_DRAW );
		glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		_psize = P.size(); // after this line array P will be deallocated (data has been sent to graphics card)

		if ( multicolor )
		{	GsArray<GsColor> C;
			C.size ( _psize ); // one color per vertex
			bool newcolorpervertex=true;
			for ( gsuint i=0; i<_psize; i++ )
			{	if ( newcolorpervertex || i%3==0 )
					C[i] = GsColor::random();
				else
					C[i] = C[i-1];
			}
			glEnableVertexAttribArray ( 1 );
			glBindBuffer ( GL_ARRAY_BUFFER, _glo.buf[1] );
			glBufferData ( GL_ARRAY_BUFFER, C.sizeofarray(), C.pt(), GL_STATIC_DRAW );
			glVertexAttribPointer ( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
		}
	}

	// 2. Enable/bind needed elements and draw:
	if ( _psize )
	{	GS_TRACE2 ( "Rendering my node..." );
		const GlProgram* Prog = multicolor? ProgMC : ProgSC; // get correct program
		ctx->use_program ( Prog->id ); // ctx tests if the program is being changed

		glUniformMatrix4fv ( Prog->uniloc[0], 1, GLTRANSPMAT, ctx->projection()->e );
		glUniformMatrix4fv ( Prog->uniloc[1], 1, GLTRANSPMAT, ctx->modelview()->e );

		if ( !multicolor ) glUniform4fv ( Prog->uniloc[2], 1, s->color().vec4() );

		glBindVertexArray ( _glo.va[0] );
		glDrawArrays ( GL_TRIANGLES, 0, _psize );
	}

	glBindVertexArray ( 0 ); // done - this call protects our data from subsequent OpenGL calls
}

//================================ EOF =================================================
