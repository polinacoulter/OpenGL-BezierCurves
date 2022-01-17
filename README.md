Enter the command `make run` to launch the program.
Right click for menu with options to add curve or set the color of the last clicked point's curve.
- Point2 class holds a 2D point comprised of 2 floats
- Bezier class consists of and array of 4 Point2 objects.
- Bezier class has the following methods:
    - Point2 Evaluate(float t); // given parameter t between 0 and 1, finds the interpolated point.
    - void DrawCurve(); // draws the curve by approximating it at about 20 evaluation points.
    - void DrawControlPoints(); // draws the 4 control points as circles.
    - int IsPicked(float x,float y);
      - // returns <n> if x,y is within <radius> of control point <n>.
      - // returns -1 if x,y is not close to any of the 4 control points.
- Display method calls DrawCurve and DrawControlPoints
- The formula used for evaluating a Bezier curve:
    - p= p0*(1-t)*(1-t)*(1-t) + 3*p1*(1-t)*(1-t)*t + 3*p2*(1-t)*t*t + p3*t*t*t;

- The user is able to move the control points around with their mouse
    - When the mouse goes does, check to see if its on a control point
    - If it is, allow the mouse to drag that point, until the mouse button is released.
    - Curve dynamically updates as its control points are altered.
