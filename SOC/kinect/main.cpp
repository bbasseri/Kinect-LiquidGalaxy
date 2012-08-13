#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#define CONFIG_PATH "data/config.xml"

#define CHECK_RC(rc, what) 								\
	if(rc != XN_STATUS_OK) { 							\
		fprintf(stderr, "%s failed: %s\n", what, xnGetStatusString(rc));	\
		return rc;								\
	}

extern void update_control(XnFloat deltaX, XnFloat deltaY, XnFloat deltaZ);
extern void init_control(const char *serveraddr, int port);

xn::Context g_Context;
xn::ScriptNode g_ScriptNode;
xn::DepthGenerator g_DepthGenerator;
xn::GestureGenerator g_GestureGenerator;
xn::HandsGenerator g_HandsGenerator;

XnUserID hand = -1;
XnPoint3D start;

/* Start tracking the hand once a gesture is recognized */
void XN_CALLBACK_TYPE Gesture_Recognized(xn::GestureGenerator &generator,
					 const XnChar *gesture,
					 const XnPoint3D *startPosition,
					 const XnPoint3D *endPosition,
					 void *cookie) {
	printf("Gesture recognized: %s\n", gesture);

	// Only start tracking a hand if we're not already tracking one.
//	if(hand < 0) {
		printf("Okay...\n");
		g_HandsGenerator.StartTracking(*endPosition);
//	}
}

/* We don't care about progress of a gesture. */
void XN_CALLBACK_TYPE Gesture_Process(xn::GestureGenerator &generator,
				      const XnChar *gesture,
				      const XnPoint3D *position,
				      XnFloat progress,
				      void *cookie) {
}

/* Record the position of a new hand. This will be the reference point for ... */
void XN_CALLBACK_TYPE Hand_Create(xn::HandsGenerator &generator,
				  XnUserID handId,
				  const XnPoint3D *position,
				  XnFloat time,
				  void *cookie) {
	printf("New Hand: %d @(%f, %f, %f)\n", handId, position->X, position->Y, position->Y
);

	hand = handId;
	start = *position;
}

/* When the hand moves, update the controls sent to fakenav. */
void XN_CALLBACK_TYPE Hand_Update(xn::HandsGenerator &generator,
				  XnUserID handId,
				  const XnPoint3D *position,
				  XnFloat time,
				  void *cookie) {

	if(handId == hand) {
		XnFloat deltaX = start.X - position->X;
		XnFloat deltaY = start.Y - position->Y;
		XnFloat deltaZ = start.Z - position->Z;

		update_control(deltaX, deltaY, deltaZ);
	}
}

/* Hand is gone, should stop updating fakenav... */
void XN_CALLBACK_TYPE Hand_Destroy(xn::HandsGenerator &generator,
				   XnUserID handId,
				   XnFloat time,
				   void *cookie) {
	printf("Lost hand: %d\n", handId);

	hand = -1;
}

int main(int argc, char *argv[]) {
	XnStatus rc;
	xn::EnumerationErrors errors;
	XnCallbackHandle cbhandle;

	if(argc < 3) {
		fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
		return 1;
	}

	//Connect to fakenav server.
	init_control(argv[1], atoi(argv[2]));

	//Create context from settings file.
	rc = g_Context.InitFromXmlFile(CONFIG_PATH, g_ScriptNode, &errors);
	if(rc == XN_STATUS_NO_NODE_PRESENT) {
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		fprintf(stderr, "%s\n", strError);
		return rc;
	} else if(rc != XN_STATUS_OK) {
		fprintf(stderr, "Open failed: %s\n", xnGetStatusString(rc));
		return rc;
	}

	//Create generators.
	rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(rc, "Find DepthGenerator");

	rc = g_GestureGenerator.Create(g_Context);
	CHECK_RC(rc, "Create GestureGenerator");

	rc = g_HandsGenerator.Create(g_Context);
	CHECK_RC(rc, "Create HandsGenerator");

	//Register callabcks.
	rc = g_GestureGenerator.RegisterGestureCallbacks(Gesture_Recognized, Gesture_Process, NULL, cbhandle);
	CHECK_RC(rc, "Register gesture callabcks");

	rc = g_HandsGenerator.RegisterHandCallbacks(Hand_Create, Hand_Update, Hand_Destroy, NULL, cbhandle);
	CHECK_RC(rc, "Register hand callbacks");

	//Add gestures.
	rc = g_Context.StartGeneratingAll();
	CHECK_RC(rc, "Generating");

	rc = g_GestureGenerator.AddGesture("Wave", NULL);
	CHECK_RC(rc, "Add gesture");

	while(1) {
		g_Context.WaitOneUpdateAll(g_DepthGenerator);
	}
}
