package com.example.application;

import android.Manifest;
import android.view.View;

public class MainActivity extends BaseActivity implements View.OnClickListener {

	ImageView img;

	public class Data
	{
		private String name;
		private String number;
	
		void setData(String name, String number){
			this.name = name;
			this.number = number;
		}
	}

	@Override
	protected void onCreate( Bundle savedInstanceState ) {

		super.onCreate( savedInstanceState );
		setContentView( R.layout.activity_main );

		img = (ImageView) findViewById(R.id.image);
		img.setOnClickListener(this);

	}

	public void onClick(View v){

		switch(v){
			case R.id.image:
				Data d = new Data();
				String name = sourceFunc();
				d.set(name, getNumber());
				sinkFunc(d);
				break;
		}

	}

}