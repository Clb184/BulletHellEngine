function main(){
    print("test starting...\n");
    wait(10);
    local e = Enemy(testing, 320.0, 100.0, 70.0, 5);
    wait(30);
    e.Kill();
    e = null;
    local l = ____enm.len();
    while (1) {
        wait(10);
        Enemy(testing, 100.0, 300.0, 70.0, tex);
        Enemy(testing, 400.0, 300.0, 70.0, tex2);
        //print("e: " + e + "\n");
        //e.Kill();
        wait(300);
    }
    wait(2000);
}

tex <- Texture("graph/dialogue_morgan.tga");
tex2 <- Texture("graph/yuuta.tga");

function testing(x, y, c, tex) {
    texture = tex;
    //print("a is: " + a + " b is: " + b + " c is: " + c + " d is: " + d + "!\n");
    SetPos(vec2t(x, y));
    SetScale(vec2t(1.0, 1.0));
    SetSize(vec2t(256.0, 256.0));
    SetUV(vec4t(0.0, 0.0, 0.7, 0.7));
    for (local i = 0; i < 300; i++){
        SetPos(vec2t(x, y + sin(i * (3.14159 / 180.0) * 2.0) * c));
        wait(1);
    }
    Kill()
    //print("goodbye!");
}