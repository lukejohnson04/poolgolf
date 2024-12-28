
v2 ProjectShapeOntoAxis(v2 A[4], v2 axis) {
    double min = V2DotProduct(axis, A[0]);
    double max = min;
    for (int i=1; i<4; i++) {
        double p = V2DotProduct(axis, A[i]);
        if (p < min) {
            min = p;
        } else if (p > max) {
            max = p;
        }
    }
    return v2((float)min, (float)max);
}

void RotateShape(v2 A[4], v2 origin, float rad) {
    for (int i=0; i<4; i++) {
        v2 p = A[i];
        v2 segment = p - origin;
        v2 rotated = V2Rotate(segment, rad);
        rotated += origin;
        A[i] = rotated;
    }
}

bool DoProjectionsOverlap(v2 A, v2 B) {
    return (A.x < B.y && A.x > B.x) || (B.x < A.y && B.x > A.x);
}

bool SATCollision(v2 A[4], v2 B[4]) {
    v2 a_axes[4];
    v2 b_axes[4];

    for (int i=0; i<4; i++) {
        v2 p1 = A[i];
        v2 p2 = A[i + 1 == 4 ? 0 : i + 1];

        v2 edge = p1 - p2;
        v2 normal = Perpendicular(edge);

        a_axes[i] = normal;
    }

    for (int i=0; i<4; i++) {
        v2 p1 = B[i];
        v2 p2 = B[i + 1 == 4 ? 0 : i + 1];

        v2 edge = p1 - p2;
        v2 normal = Perpendicular(edge);

        b_axes[i] = normal;
    }

    // Project onto axes
    for (int i=0; i<4; i++) {
        v2 axis = a_axes[i];
        v2 p1 = ProjectShapeOntoAxis(A, axis);
        v2 p2 = ProjectShapeOntoAxis(B, axis);

        if (!DoProjectionsOverlap(p1, p2)) {
            return false;
        }
    }


    // Project onto axes
    for (int i=0; i<4; i++) {
        v2 axis = b_axes[i];
        v2 p1 = ProjectShapeOntoAxis(A, axis);
        v2 p2 = ProjectShapeOntoAxis(B, axis);

        if (!DoProjectionsOverlap(p1, p2)) {
            return false;
        }
    }
    
    return true;
}

// Rotates shape A by rad and stores in B
void RotateAABB(fRect A, v2 B[4], float rad, v2 origin) {
    B[0] = v2(A.x,A.y);
    B[1] = v2(A.x+A.w,A.y);
    B[2] = v2(A.x+A.w,A.y+A.h);
    B[3] = v2(A.x,A.y+A.h);

    RotateShape(B, origin, rad);
}

void RotateAABB(fRect A, v2 B[4], float rad) {
    RotateAABB(A,B,rad,v2(A.x+A.w/2.f,A.y+A.h/2.f));
}
