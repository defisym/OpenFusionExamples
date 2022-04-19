package _DeLib;

import _DeLib.Coord.*;

import java.util.Vector;
import java.util.function.BiPredicate;

import static _DeLib.Coord.VecContains;
import static _DeLib.FusionUtilities.swap;
import static java.lang.Math.min;
import static java.lang.Math.max;

public class AOEClass {
    public final static int RESERVE = 50;

    private final Coord[] dirOffset = { new Coord(1, 0, true), new Coord(0, -1, true), new Coord(-1, 0, true),
            new Coord(0, 1, true) };

    private Coord start = new Coord(0, 0, true);
    private int dir = 0;

    private Vector<Coord> open_set = new Vector<>();
    private Vector<Coord> close_set = new Vector<>();
    private Vector<Coord> cur_set = new Vector<>();

    public void GetAOE_1_X(int size, Vector<Coord> output) {
        for (int i = 1; i <= size; i++) {
            output.add(start.add(new Coord(dirOffset[dir].x * (int) i, dirOffset[dir].y * (int) i, true)));
        }
    }

    public void GetAOE_2_X(int type, Vector<Coord> output) {
        int vSize = 0;
        int hSize = 0;

        switch (type) {
            case 21:
                vSize = 1;
                break;
            case 22:
                vSize = 2;
                break;
            case 23:
                vSize = 1;
                break;
            case 24:
                vSize = 2;
                break;
            case 25:
                vSize = 3;
                break;
            case 26:
                vSize = 3;
                break;
        }

        for (int v = 0; v < vSize; v++) {
            switch (type) {
                case 21:
                    hSize = 3;
                    break;
                case 22:
                    hSize = 3;
                    break;
                case 23:
                    hSize = 5;
                    break;
                case 24:
                    hSize = 5;
                    break;
                case 25:
                    hSize = 5 - 2 * v;
                    break;
                case 26:
                    hSize = 1 + 2 * v;
                    break;
            }

            for (int h = 0; h < hSize; h++) {
                int x = 0;
                int y = 0;

                if (dir == 1 || dir == 3) {
                    x = h - (hSize >> 1);
                    y = -1 * (1 + v) * (2 - dir);
                } else {
                    x = (1 + v) * (1 - dir);
                    y = h - (hSize >> 1);
                }

                output.add(start.add(new Coord(x, y, true)));
            }
        }
    }

    public void GetAOE_3_X(int size, Vector<Coord> output) {
        open_set.clear();
        close_set.clear();
        cur_set.clear();

        cur_set.add(start);

        for (int i = 0; i <= size; i++) {
            swap(open_set, cur_set);

            while (!open_set.isEmpty()) {
                Coord base = open_set.lastElement();
                open_set.remove(open_set.size() - 1);

                BiPredicate<Vector<Coord>, Coord> find = Coord::VecContains;

                if (!find.test(close_set, base)) {
                    close_set.add(base);
                }

                for (Coord it : dirOffset) {
                    Coord cur = new Coord(it.x + base.x, it.y + base.y, true);

                    if (!find.test(close_set, cur)) {
                        cur_set.add(cur);
                    }
                }
            }
        }

        for (int i = 1; i < close_set.size(); i++) {
            output.add(close_set.get(i));
        }

        return;
    }

    public void GetAOE_4_X(int size, Vector<Coord> output) {
        output.add(start.add(new Coord(1, -1, true)));
        output.add(start.add(new Coord(-1, -1, true)));
        output.add(start.add(new Coord(-1, 1, true)));
        output.add(start.add(new Coord(1, 1, true)));
    }

    public AOEClass() {
        open_set.ensureCapacity(RESERVE);
        close_set.ensureCapacity(RESERVE);
        cur_set.ensureCapacity(RESERVE);
    }

    public void GetAOE(Coord start, int dir, int type, Vector<Coord> output) {
        output.clear();

        this.start = start;
        this.dir = max(0, min(3, dir));

        switch (type / 10) {
            case 2:
                GetAOE_2_X(type, output);
                return;
        }

        switch (type) {
            case 11:
                GetAOE_1_X(2, output);
                return;
            case 12:
                GetAOE_1_X(4, output);
                return;
            case 31:
                GetAOE_3_X(2, output);
                return;
            case 32:
                GetAOE_3_X(3, output);
                return;
            case 41:
                GetAOE_4_X(0, output);
                return;
        }
    }

}
