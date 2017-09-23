package com.example.antheali.c2_23;

import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.*;

public class MainActivity extends AppCompatActivity {
    private Button btn;
    private TextView light;
    private int count = 0;
    private boolean i = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        btn = (Button) findViewById(R.id.btn);
        light = (TextView) findViewById(R.id.light);

        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view){

                if(i==false){
                    i=!i;
                    btn.setText("NEXT");
                    light.setBackgroundColor(Color.rgb(250, 0, 0));
                    count++;
                }
                else {
                    if (count % 3 == 0) {
                        light.setBackgroundColor(Color.rgb(250, 0, 0));
                        count++;

                    } else if (count % 3 == 1) {
                        light.setBackgroundColor(Color.rgb(250, 250, 0));
                        count++;
                    } else {
                        light.setBackgroundColor(Color.rgb(0, 220, 0));
                        count++;
                    }
                }

            }


        });


    }
}
