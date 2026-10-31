package ofe.sdlprobe;
import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.ParcelFileDescriptor;
import java.io.File;
import java.io.FileNotFoundException;
/** Private test provider, exercising real ContentResolver reads without path conversion. */
public final class ProbeProvider extends ContentProvider {
    public boolean onCreate(){return true;}
    @Override public ParcelFileDescriptor openFile(Uri uri,String mode)throws FileNotFoundException {
        String name=uri.getLastPathSegment();
        if(!"声音 🎵.wav".equals(name)&&!"video.mp4".equals(name))throw new FileNotFoundException(name);
        return ParcelFileDescriptor.open(new File(getContext().getFilesDir(),name),ParcelFileDescriptor.MODE_READ_ONLY);
    }
    public String getType(Uri uri){return "application/octet-stream";}
    public Cursor query(Uri uri,String[] projection,String selection,String[] args,String sort){return null;}
    public Uri insert(Uri uri,ContentValues values){throw new UnsupportedOperationException();}
    public int delete(Uri uri,String selection,String[] args){throw new UnsupportedOperationException();}
    public int update(Uri uri,ContentValues values,String selection,String[] args){throw new UnsupportedOperationException();}
}
