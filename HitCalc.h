#ifndef HITCALC_INCLUDED
#define HITCALC_INCLUDED

#include <cmath>
#include <memory>

struct CollissionShape {
	union {
		struct {
			float x;
			float y;
		};
		glm::vec2 pos;
	};

};

struct CollissionRect : public CollissionShape{
	float w = 48.0f;
	float h = 48.0f;
};

struct CollissionRectRot : public CollissionRect {
	float d = 0.0f;
	CollissionRect prerect;
};

struct CollissionCircle : public CollissionShape {
	float r = 48.0f;
};

inline CollissionRect _vectorcall getRectFromProjection(const CollissionRectRot& target, float cd, float cx, float cy) {

	float newAng1 = target.d + cd;
	float a = fabsf(cosf(newAng1));
	float b = fabsf(sinf(newAng1));
	float vecx = target.x - cx;
	float vecy = target.y - cy;
	float ang_ex = atan2f(vecy, vecx);
	float dist = sqrtf(vecx * vecx + vecy * vecy);
	vecx = dist * cosf(ang_ex - cd);
	vecy = dist * sinf(ang_ex - cd);

	return {
		vecx + cx, vecy + cy,
		a * target.w + b * target.h,
		b * target.w + a * target.h
	};
}


//Non-rotated boxes hit check
inline bool _vectorcall hitCheckNRBNRB(const CollissionRect& enm1, const CollissionRect& enm2) {
	/*const float ew = enm1.m_HitWidth * 0.5f,
				eh = enm1.m_HitHeight * 0.5f,
				enm_x_now = std::abs(enm2.m_x - enm1.m_x) - enm2.m_HitWidth * 0.5f,
				enm_y_now = std::abs(enm2.m_y - enm1.m_y) - enm2.m_HitHeight * 0.5f;
				*/
	/*
	|		 ______
	|		|	  |
	|		|_____|
 h/2|_____
	|	  |
	______|____________________
	w/2
	*/
	//return (enm_x_now < ew&& enm_y_now < eh);



	return 
	fabsf(enm1.x - enm2.x) * 2.0f < enm1.w + enm2.w &&
	fabsf(enm1.y - enm2.y) * 2.0f < enm1.h + enm2.h;
}
	
inline bool _vectorcall hitCheckBoxes(const CollissionRectRot& enm1, const CollissionRectRot& enm2) {
	return  hitCheckNRBNRB(getRectFromProjection(enm2, enm1.d, enm1.x, enm1.y), (CollissionRect)enm1) && hitCheckNRBNRB(getRectFromProjection(enm1, enm2.d, enm2.x, enm2.y), (CollissionRect)enm2);
}

inline bool _vectorcall hitCheckCC(const CollissionCircle& enm1, const CollissionCircle& enm2) {
	float c0 = enm2.x - enm1.x;
	float c1 = enm2.y - enm1.y;
	float h = enm2.r + enm1.r;

	return (c0 * c0 + c1 * c1) <= (h * h);
}

inline bool _vectorcall hitCheckRectCircle(const CollissionCircle& c, const CollissionRectRot& r) {
	float vecx = c.x - r.x;
	float vecy = c.y - r.y;
	float ang_ex = atan2f(vecy, vecx);
	float dist = sqrtf(vecx * vecx + vecy * vecy);
	vecx = dist * cosf(ang_ex - r.d);
	vecy = dist * sinf(ang_ex - r.d);

	float hw = r.w * 0.5f;
	float hh = r.h * 0.5f;

	// temporary variables to set edges for testing
	//float tempn = -hw;
	//float tempp = hw;

	//number32_t n;
	//n.f = (vecx + hw);
	float testX = (vecx < -hw) ? -hw : (vecx > hw) ? hw : vecx;
	//testX = (vecx < -hw) * -hw + (vecx >= hw) * hw;
//tempn = -hh;
//tempp = hh;
	float testY = (vecy < -hh) ? -hh : (vecy > hh) ? hh : vecy;
	//testY = ((vecy < -hh) >> 24 &) * -hh + (vecy >= hh) * hh;
// get distance from closest edges
	float distX = vecx - testX;
	float distY = vecy - testY;

	// if the distance is less than the radius, collision!
	return (distX * distX + distY * distY) <= c.r * c.r;
}

#endif