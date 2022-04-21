function FusionUtilities() {

}

FusionUtilities.prototype = {
    GetEvtParam: function (evt, rhPtr, _num) {
        return evt.evtParams[_num];
    },

    GetPositionInfo: function (rhPtr, param) {
        var pInfo = new CPositionInfo();
        param.read_Position(rhPtr, 0x11, pInfo);

        return pInfo;
    },

    GetPositionInfo: function (evt, rhPtr, _num) {
        var param = evt.evtParams[_num];

        return GetPositionInfo(rhPtr, param);
    },

    GetOil: function (evt, rhPtr, _num) {
        var param = evt.evtParams[_num];
        return param.oiList;
    },

    IsNegated: function (cnd) {
        return (cnd.evtFlags2 & CEvent.EVFLAG2_NOT) != 0;
    },

    LPROValid: function (object, identifier=0) {
        if (object == null) {
            return false;
        }

        if (identifier != 0) {
            return object.hoIdentifier == identifier;
        }

        return true; // need not to check identifier
    },

    FixedVal: function (object) {
        return (object.hoCreationId << 16) | ((object.hoNumber) & 0xFFFF);
    },

    LproFromFixed: function (rhPtr, fixed) {
        var ObjectList = rhPtr.hoAdRunHeader.rhObjectList;
        var id = 0x0000FFFF & fixed;

        return ObjectList[id];
    },

    GenerateBitmapFromHandle: function(rhPtr, hImage) {
        var cImage = rhPtr.getImageBank().getImageFromHandle(hImage);

        return GenerateBitmap(cImage);
    },

    GenerateBitmap: function(cImage) {
        var mImage = cImage.getRawPixels();

        if (mImage == null) {
            return null;
        }

        var image = Bitmap.createBitmap(cImage.getWidth(), cImage.getHeight(), cImage.getFormat());
        image.setPixels(mImage, 0, cImage.getWidth(), 0, 0, cImage.getWidth(), cImage.getHeight());

        return image;
    }
}
