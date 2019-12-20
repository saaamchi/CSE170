# include "sn_mynode.h"

const char* SnMyNode::class_name = "SnMyNode"; // static
SN_SHAPE_RENDERER_DEFINITIONS(SnMyNode);

//===== SnMyNode =====

SnMyNode::SnMyNode() : SnShape(class_name)
{
	width = 3.0f;
	height = 4.0f;
	radius = 0.7f;
	R = 15.0f;
	n = 40;
	multicolor = true;
	if (!SnMyNode::renderer_instantiator) SnMyNodeRegisterRenderer();
}

SnMyNode::~SnMyNode()
{
}

void SnMyNode::get_bounding_box(GsBox& b) const
{
	// the bounding box is needed for camera view_all computations
	b.set(init, init + GsVec(width, height, 0));
}

//===== Renderer Instantiator =====

# include "glr_mynode.h"

static SnShapeRenderer* GlrMyNodeInstantiator()
{
	return new GlrMyNode;
}

void SnMyNodeRegisterRenderer()
{
	SnMyNode::renderer_instantiator = &GlrMyNodeInstantiator;
}
