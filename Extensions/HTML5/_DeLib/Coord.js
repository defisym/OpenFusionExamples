Coord.MAX_VALUE = 0x7fffffff;

function Coord(x, y, minus) {
    if (arguments.length === 2) {
        this.x = x>=0?x:MAX_VALUE;
        this, y = y>=0?y:MAX_VALUE;
    }
    if (arguments.length === 3) {
        this.x = x;
        this, y = y;
    }
}

Coord.prototype = {
    isEqual: function(coord) {
        return this.x===coord.x && this.y===coord.y;
    }
}