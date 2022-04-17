package _DeLib;

public class AOECoord {

    int x;
    int y;

    public AOECoord(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public AOECoord add (AOECoord other) {
        this.x += other.x;
        this.y += other.y;

        return other;
    }

    public AOECoord sub (AOECoord other) {
        this.x -= other.x;
        this.y -= other.y;

        return other;
    }

    public AOECoord mul (int other) {
        this.x *= other;
        this.y *= other;

        return this;
    }

    public AOECoord div (int other) {
        this.x /= other;
        this.y /= other;

        return this;
    }

    public boolean isEqual (AOECoord other) {
        return this.x == other.x && this.y == other.y;
    }
}
