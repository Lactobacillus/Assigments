package com.df.tastemakers.applicataion;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.net.Uri;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.df.tastemakers.R;
import com.df.tastemakers.applicataion.fragment.PictureDialogFragment;
import com.df.tastemakers.applicataion.server.ServerQuery;
import com.df.tastemakers.applicataion.server.response.ResponseCheckInCompleteJson;
import com.df.tastemakers.applicataion.server.response.ResponseGameBCheckInJson;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;
import com.tsengvn.typekit.TypekitContextWrapper;

import java.io.File;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class CheckInActivity extends AppCompatActivity implements View.OnClickListener {

    private Button ok_btn;
    private EditText edit;
    private File file;
    private RecyclerView image_view;
    private CheckInImageAdapter mAdapter;
    private PictureDialogFragment mPictureDialog;
    private int memberId;
    private int gentleMapId;
    private int shopId;
    private int missionNo;
    File[] temp = new File[5];

    @Override
    protected void attachBaseContext(Context newBase) {
        super.attachBaseContext(TypekitContextWrapper.wrap(newBase));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_check_in);
        setCustomTitle();

        FacebookSdk.sdkInitialize(getApplicationContext());
        CallbackManager callbackManager = create();

        SharedPreferences pref = getSharedPreferences("pref", MODE_PRIVATE);
        memberId = pref.getInt("userId", 0);
        gentleMapId = getIntent().getIntExtra("gentleMapId", 0);
        shopId = getIntent().getIntExtra("shopId", 0);
        missionNo = getIntent().getIntExtra("missionNo", 0);
        TextView category = (TextView) findViewById(R.id.list_title_text);
        category.setText(getIntent().getStringExtra("shopCategory"));
        TextView count = (TextView) findViewById(R.id.check_in_count);
        count.setText(getIntent().getIntExtra("shopCount",0)+"");
        TextView title = (TextView) findViewById(R.id.check_in_title);
        title.setText(getIntent().getStringExtra("shopName"));
        String url = getIntent().getStringExtra("shopImage");
        ImageView img = (ImageView) findViewById(R.id.list_image);
        if(img.getDrawable()==null){
            Glide.with(getApplicationContext())
                    .load(url)
                    .into(img);
        }

        TextView toolbar = (TextView) findViewById(R.id.toolbar_title);
        toolbar.setText(getIntent().getStringExtra("title"));

        TextWatcher textwatcher = new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                String str = s.toString();
                if(str.length()>0 && mAdapter.mDataset.size()>=1){
                    ok_btn.setBackgroundColor(Color.parseColor("#dd0d0d"));
                    ok_btn.setTextColor(Color.parseColor("#ffffff"));
                    //ok_btn.setEnabled(true);
                }
                else{
                    ok_btn.setBackgroundColor(Color.parseColor("#333333"));
                    ok_btn.setTextColor(Color.parseColor("#808080"));
                    //ok_btn.setEnabled(false);
                }
            }
        };

        edit = (EditText) findViewById(R.id.checkin_edit);
        edit.addTextChangedListener(textwatcher);
        edit.setFocusableInTouchMode(true);


        ok_btn = (Button)findViewById(R.id.ok_button);
        ok_btn.setOnClickListener(this);
        ImageView camera_btn = (ImageView)findViewById(R.id.btn_camera);

        camera_btn.setOnClickListener(this);

        image_view = (RecyclerView) findViewById(R.id.image_recycler_view);
        LinearLayoutManager layoutManager
                = new LinearLayoutManager(this, LinearLayoutManager.HORIZONTAL, false);
        image_view.setLayoutManager(layoutManager);

        mAdapter = new CheckInImageAdapter(CheckInActivity.this);
        image_view.setAdapter(mAdapter);

        ShareDialog shareDialog = new ShareDialog(CheckInActivity.this);
                                            shareDialog.registerCallback(callbackManager, new FacebookCallback<Sharer.Result>() {
                                                @Override
                                                public void onSuccess(Sharer.Result result) {

                                                    ServerQuery.goCheckInComplete(responseGameBCheckInJson.reviewId, gentleMapId, missionNo, true, new Callback() {
                                                        @Override
                                                        public void onResponse(Call call, Response response) {
                                                            if (response.isSuccessful()) {
                                                                ResponseCheckInCompleteJson response1 = (ResponseCheckInCompleteJson) response.body();
                                                                if (response1.success) {

                                                                    int i;
                                                                    for (i = 0; i < temp.length; i++) {
                                                                        if (temp[i] != null) {
                                                                            if (temp[i].exists())
                                                                                temp[i].delete();
                                                                        }
                                                                    }

                                                                    Intent intent = new Intent();
                                                                    intent.putExtra("complete", "complete");
                                                                    setResult(123, intent);
                                                                    finish();
                                                                } else {
                                                                }
                                                            } else {
                                                            }
                                                        }

                                                        @Override
                                                        public void onFailure(Call call, Throwable t) {
                                                        }
                                                    });

                                                }

                                                @Override
                                                public void onCancel() {

                                                    ServerQuery.goCheckInComplete(responseGameBCheckInJson.reviewId, gentleMapId, missionNo, false, new Callback() {
                                                        @Override
                                                        public void onResponse(Call call, Response response) {
                                                            if (response.isSuccessful()) {
                                                                ResponseCheckInCompleteJson response1 = (ResponseCheckInCompleteJson) response.body();
                                                                if (response1.success) {
                                                                    Toast.makeText(getApplicationContext(), "페이스북에 공유하지 않을 경우 체크인이 완료되지 않습니다.", Toast.LENGTH_SHORT)
                                                                            .show();
                                                                } else {
                                                                }
                                                            } else {
                                                            }
                                                        }

                                                        @Override
                                                        public void onFailure(Call call, Throwable t) {
                                                        }
                                                    });

                                                }

                                                @Override
                                                public void onError(FacebookException error) {

                                                }
                                            });
                                            shareDialog.show(content);

    }

    public void onClick(View v){
        switch(v.getId()){
            case R.id.ok_button:
                if(edit.getText().toString().length()==0){
                    Toast.makeText(getApplicationContext(),"리뷰가 작성되지 않았습니다.", Toast.LENGTH_SHORT).show();
                } else if (mAdapter.mDataset.size()==0) {
                    Toast.makeText(getApplicationContext(),"리뷰 이미지가 첨부되지 않았습니다.", Toast.LENGTH_SHORT).show();
                } else {

                    int i;
                    for (i = 0; i < 5; i++) {
                        if (i < mAdapter.mFile.size())
                            temp[i] = mAdapter.mFile.get(i);
                        else
                            temp[i] = null;
                    }
                    ServerQuery.goGameBCheckIn(memberId, gentleMapId, shopId, missionNo, temp[0], temp[1],
                            temp[2], temp[3], temp[4], edit.getText().toString(), new Callback() {
                                @Override
                                public void onResponse(Call call, Response response) {
                                    if (response.isSuccessful()) {
                                        final ResponseGameBCheckInJson responseGameBCheckInJson = (ResponseGameBCheckInJson) response.body();
                                        if (responseGameBCheckInJson.success) {

                                            String url = "http://k6vgp.app.goo.gl/?link="
                                                    +"http://sns.tastemakersofficial.co.kr/gentle_map/check_in?checkin_id=" + gentleMapId + "/"
                                                    + responseGameBCheckInJson.reviewId + "&apn=com.df.tastemakers&al=tastemakers://gentle_map/check_in?checkinId="
                                                    + gentleMapId + "/" + responseGameBCheckInJson.reviewId + "&ibi=com.tastemakers&ius=tastemakers&ifl=http://tastemakersofficial.co.kr/";

                                            ShareLinkContent content = new ShareLinkContent.Builder()
                                                    .setContentUrl(Uri.parse(url))
                                                    .build();

                                        } else {
                                            Toast.makeText(getApplicationContext(), responseGameBCheckInJson.message, Toast.LENGTH_LONG);
                                        }
                                    } else {
                                        Toast.makeText(getApplicationContext(), "체크인 실패", Toast.LENGTH_LONG);
                                    }
                                }

                                @Override
                                public void onFailure(Call call, Throwable t) {
                                    Toast.makeText(getApplicationContext(), "체크인 실패", Toast.LENGTH_LONG);
                                }
                            });
                }
                break;
            case R.id.btn_camera:

                if(mAdapter.mDataset.size()>=5){
                    Toast.makeText(getApplicationContext(),"리뷰 이미지는 최대 5장까지 등록이 가능합니다.",Toast.LENGTH_SHORT).show();
                }else {
                    mPictureDialog = new PictureDialogFragment().setListener(new JoinActivity.PicturePickListener() {
                        @Override
                        public void isSuccess(File files) {
                            file = files;
                            Bitmap bitmap = BitmapFactory.decodeFile(file.getPath());
                            Log.d("Camera", "OK");
                            mAdapter.mDataset.add(new ImageData(bitmap));
                            mAdapter.mFile.add(file);
                            mAdapter.notifyDataSetChanged();
                            if (!edit.getText().toString().isEmpty()) {
                                ok_btn.setBackgroundColor(Color.parseColor("#dd0d0d"));
                                ok_btn.setTextColor(Color.parseColor("#ffffff"));
                                ok_btn.setEnabled(true);
                            }
                        }

                        @Override
                        public void isFail() {
                            Log.d("Camera", "FAILED");

                        }
                    });

                    FragmentManager manager = getSupportFragmentManager();
                    mPictureDialog.show(manager, null);
                }

                break;
            case R.id.back:
                finish();
                break;
        }
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        callbackManager.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onResume() {
        edit.clearFocus();
        super.onResume();
    }

    public void setCustomTitle(){
        RelativeLayout btn = (RelativeLayout) findViewById(R.id.back);
        btn.setOnClickListener(this);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
    }
}