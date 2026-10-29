package _DeLib;

import java.util.Vector;

/** Thin adapter to the same AOE.h used by Windows and future iOS bindings. */
public final class AOEClass {
    public void GetAOE(Coord start, int dir, int type, Vector<Coord> output) {
        int[] coords = FindTheWayClass.aoe(start, Math.max(0, Math.min(3, dir)), type);
        output.clear();
        for (int i = 0; i < coords.length; i += 2) {
            output.add(new Coord(coords[i], coords[i + 1], true));
        }
    }
}
