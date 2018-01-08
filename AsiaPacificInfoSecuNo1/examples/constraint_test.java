public class Data{

	String f;

	String get(){

		return this.f;
	}

	void set(String p){

		this.f = p;
	}
}
public class FieldSensitivity3{

	protected void onCreate(Bundle b){

		Data dt = new Data();
		String sim = tm.getSimSerialNumber(); // source
		dt.set(sim);
		String sg = dt.get();
		sms.sendTextMessage(sg); // sink
	}
}