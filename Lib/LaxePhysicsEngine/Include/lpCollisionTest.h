#pragma once

#include "export.h"
#include <vector>

#include "lpNarrowPhase.h"
#include "lpContactJoint.h"
#include "lpRigidBody.h"



void lpCollisions(lpCollisionGeometry *g1, lpCollisionGeometry *g2, std::vector<lpContactJoint*> *contacts, unsigned int &numContacts);

void testGeoms(lpCollisionPrimitive *geom1, lpCollisionPrimitive *geom2, std::vector<lpContactJoint*> *contacts, unsigned int &numContacts);

void testSphereSphere(lpSphere *s1, lpSphere *s2, std::vector<lpContactJoint*> *contacts, unsigned int &numContacts);
void testBoxSphere(lpBox *b, lpSphere *s, std::vector<lpContactJoint*> *contacts, unsigned int &numContacts);
void testBoxBox(lpBox *b1, lpBox *b2, std::vector<lpContactJoint*> *contacts, unsigned int &numContacts);

void completeContact(lpContactJoint *c, lpCollisionPrimitive *geom1, lpCollisionPrimitive *geom2);
