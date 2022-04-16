package _DeLib;

enum CoordType{
    X,
    Y,
}

public class Coord {
    public int x;
    public int y;

    public Coord(int x, int y){
        this.x = x;
        this.y = y;
    }

    public boolean isEqual(Coord coord){
        return this.x == coord.x && this.y == coord.y;
    }

    public Coord add(Coord coord){
        return new Coord(this.x + coord.x, this.y + coord.y);
    }

    public Coord add(int coord){
        return new Coord(this.x + coord, this.y + coord);
    }

    public Coord sub(Coord coord){
        return new Coord(this.x - coord.x, this.y - coord.y);
    }

    public Coord mul(int value){
        return new Coord(this.x * value, this.y * value);
    }

    public Coord div(int value){
        return new Coord(this.x / value, this.y / value);
    }

    public int get(CoordType type){
        if(type == CoordType.X){
            return this.x;
        }else{
            return this.y;
        }
    }

    public void set(CoordType type, int value){
        if(type == CoordType.X){
            this.x = value;
        }else{
            this.y = value;
        }
    }
}
