package application.example.com.myapplication;

import android.content.Context;
import android.provider.Telephony;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

class Data {
    Context f;

    Context get() {
        return this.f;
    }

    void set(Context p) {
        this.f = p;
    }

}

class Push {
    Stirng getPackageName(Context s){
        return s.aaaa();
    }
}

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Data d = new Data();
        Context sim = getApplicationContext();
        d.set(sim);
        Context sg = d.get();
        Push sink = new Push();
        sink.getPackageName(sg);
    }
}
