function ObjectSelectionClass(runApp) {
    this.rhPtr = runApp;
    this.run = this.rhPtr.run;
    this.eventProgram = this.run.rhEvtProg;
    this.ObjectList = this.run.rhObjectList;				//get a pointer to the mmf object list
    this.OiList = this.run.rhOiList;						//get a pointer to the mmf object info list
    this.QualToOiList = this.eventProgram.qualToOiList;	//get a pointer to the mmf qualifier to Oi list
}

ObjectSelectionClass.prototype = {
    GetOil: function (evt, rhPtr, _num) {
        var param = evt.Params[_num];
        return param.OiList;
    },

    //Returns the object-info structure from a given object-type
    GetOILFromOI: function (Oi) {
        for (i = 0; i < this.run.rhMaxOI; ++i) {
            var oil = this.OiList[i];
            if (oil.oilOi == Oi)
                return oil;
        }
        return null;
    },

    //Filter qualifier objects
    FilterQualifierObjects: function (rdPtr, OiList, negate, filter) {
        this.QualToOiList = this.eventProgram.qualToOiList;
        var CurrentQualToOi = this.QualToOiList[OiList];

        var hasSelected = false;
        var i = 0;

        while (i < CurrentQualToOi.qoiList.length) {
            var CurrentOi = CurrentQualToOi.qoiList[i + 1];
            hasSelected = (((hasSelected ? 1 : 0) |
                (this.FilterNonQualifierObjects(rdPtr, CurrentOi, negate, filter) ? 1 : 0))) != 0;

            i += 2;
        }
        return hasSelected;
    },

    //Filter normal objects
    FilterNonQualifierObjects: function (rdPtr, OiList, negate, filter) {
        var pObjectInfo = this.OiList[OiList];
        if (pObjectInfo == null)
            return false;
        var hasSelected = false;
        if (pObjectInfo.oilEventCount != this.eventProgram.rh2EventCount) {
            this.SelectAll(OiList);	//The SOL is invalid, must reset.
        }

        //If SOL is empty
        if (pObjectInfo.oilNumOfSelected <= 0) {
            return false;
        }

        var firstSelected = -1;
        var count = 0;
        var current = pObjectInfo.oilListSelected;
        var previous = null;

        while ((current & 0x80000000) == 0) {
            var pObject = this.ObjectList[current];
            var filterResult = filter(rdPtr, pObject);
            var useObject = ((filterResult ? 1 : 0) ^ (negate ? 1 : 0)) != 0;
            hasSelected = ((hasSelected ? 1 : 0) | (useObject ? 1 : 0)) != 0;

            if (useObject) {
                if (firstSelected == -1) {
                    firstSelected = current;
                }

                if (previous != null) {
                    previous.hoNextSelected = current;
                }

                previous = pObject;
                count++;
            }
            current = pObject.hoNextSelected;
        }
        if (previous != null) {
            previous.hoNextSelected = -1;
        }

        pObjectInfo.oilListSelected = firstSelected;
        pObjectInfo.oilNumOfSelected = count;

        return hasSelected;
    },


    //Selects *all* objects of the given object-type
    SelectAll: function (OiList) {
        var pObjectInfo = this.OiList[OiList];
        pObjectInfo.oilNumOfSelected = pObjectInfo.oilNObjects;
        pObjectInfo.oilListSelected = pObjectInfo.oilObject;
        pObjectInfo.oilEventCount = this.eventProgram.rh2EventCount;

        var i = pObjectInfo.oilObject;
        while ((i & 0x80000000) == 0) {
            var pObject = this.ObjectList[i];
            pObject.hoNextSelected = pObject.hoNumNext;
            i = pObject.hoNumNext;
        }
    },

    //Resets all objects of the given object-type
    SelectNone: function (OiList) {
        var pObjectInfo = this.OiList[OiList];
        if (pObjectInfo == null)
            return;
        pObjectInfo.oilNumOfSelected = 0;
        pObjectInfo.oilListSelected = -1;
        pObjectInfo.oilEventCount = this.eventProgram.rh2EventCount;
    },

    //Resets the SOL and inserts only one given object
    SelectOneObject: function (object) {
        var pObjectInfo = object.hoOiList;
        pObjectInfo.oilNumOfSelected = 1;
        pObjectInfo.oilEventCount = this.eventProgram.rh2EventCount;
        pObjectInfo.oilListSelected = object.hoNumber;
        this.ObjectList[object.hoNumber].hoNextSelected = -1;
    },

    //Resets the SOL and inserts the given list of objects
    SelectObjects: function (OiList, objects) {
        var pObjectInfo = this.OiList[OiList];

        if (pObjectInfo == null)
            return;

        pObjectInfo.oilNumOfSelected = objects.length;
        pObjectInfo.oilEventCount = eventProgram.rh2EventCount;

        if (objects.length == 0)
            return;

        var i = 0;
        var prevNumber = objects[i].hoNumber;
        pObjectInfo.oilListSelected = prevNumber;
        while (i < objects.length) {
            currentNumber = objects[i++].hoNumber;
            this.ObjectList[prevNumber].hoNextSelected = currentNumber;
            prevNumber = currentNumber;
        }
        this.ObjectList[prevNumber].hoNextSelected = -1;
    },

    //Run a custom filter on the SOL (via function callback)
    FilterObjects: function (rdPtr, OiList, negate, filter) {
        if ((OiList & 0x8000) != 0) {
            return ((this.FilterQualifierObjects(rdPtr, OiList & 0x7FFF, negate, filter) ? 1 : 0) ^ (negate ? 1 : 0)) != 0;
        }
        return ((this.FilterNonQualifierObjects(rdPtr, OiList, negate, filter) ? 1 : 0) ^ (negate ? 1 : 0)) != 0;
    },

    //Return the number of selected objects for the given object-type
    GetNumberOfSelected: function (OiList) {
        if ((OiList & 0x8000) != 0) {
            OiList &= 0x7FFF;	//Mask out the qualifier part
            var numberSelected = 0;

            this.QualToOiList = this.eventProgram.qualToOiList;
            var CurrentQualToOi = this.QualToOiList[OiList];

            var i = 0;
            while (i < CurrentQualToOi.qoiList.length) {
                var CurrentOi = this.OiList[CurrentQualToOi.qoiList[i + 1]];
                numberSelected += CurrentOi.oilNumOfSelected;
                i += 2;
            }
            return numberSelected;
        }
        else {
            var pObjectInfo = this.OiList[OiList];
            return pObjectInfo.oilNumOfSelected;
        }
    },

    //Return the number of selected objects for the given object-type
    GetNumberOfObject: function (OiList) {
        if ((OiList & 0x8000) != 0) {
            OiList &= 0x7FFF;	//Mask out the qualifier part
            var number = 0;

            this.QualToOiList = this.eventProgram.qualToOiList;
            var CurrentQualToOi = this.QualToOiList[OiList];

            var i = 0;
            while (i < CurrentQualToOi.qoiList.length) {
                var CurrentOi = this.OiList[CurrentQualToOi.qoiList[i + 1]];
                number += CurrentOi.oilNObjects;
                i += 2;
            }
            return number;
        }
        else {
            var pObjectInfo = this.OiList[OiList];
            return pObjectInfo.oilNObjects;
        }
    },

    ObjectIsOfType: function (obj, OiList) {
        if ((OiList & 0x8000) != 0) {
            OiList &= 0x7FFF;	//Mask out the qualifier part

            this.QualToOiList = this.eventProgram.qualToOiList;
            var CurrentQualToOi = this.QualToOiList[OiList];

            var i = 0;
            while (i < CurrentQualToOi.qoiList.length) {
                var CurrentOi = this.OiList[CurrentQualToOi.qoiList[i + 1]];
                if (CurrentOi.oilOi == obj.hoOi)
                    return true;
                i += 2;
            }
            return false;
        }
        return (obj.hoOi == this.OiList[OiList].oilOi);
    },

    ForEach: function (oiList, filter, forceAll = false) {
        var iterate = function (list,selected) {
            var num = selected ? list.oilListSelected : list.oilObject;

            while (num > 0) {
                var obj = run.rhObjectList[num];

                if (obj == null) {
                    break;
                }

                if ((obj.hoFlags & HOF_DESTROYED) == 0) {
                    filter(obj);
                }

                num = selected ? obj.hoNextSelected : obj.hoNumNext;
            }
        }

        var iterateCall=function(list){
            if (forceAll) {
                iterate(list, false);
                return;
            }

            if (list.oilEventCount == eventProgram.rh2EventCount) {
                // Object type already filtered, loop through selected list
                iterate(list, true);
            } else {
                // Object type not filtered yet, loop through all objects instead
                iterate(list, false);
            }
        }

        // Single object type
        if (oiList >= 0) {
            var pObjectInfo = this.OiList[oiList];

            if (pObjectInfo == null) {
                return;
            }

            iterateCall(pObjectInfo);
        } else if (oiList != -1) {
            oiList &= 0x7FFF;

            QualToOiList = eventProgram.qualToOiList;
            var CurrentQualToOi = QualToOiList[oiList];

            if (CurrentQualToOi == null) {
                return;
            }

            var i = 0;
            var length = CurrentQualToOi.qoiList.length;
            while (i < length) {
                var CurrentOi = this.OiList[CurrentQualToOi.qoiList[i + 1]];

                if (CurrentOi == null) {
                    return;
                }

                iterateCall(CurrentOi);

                i += 2;
            }

        }
    },

}