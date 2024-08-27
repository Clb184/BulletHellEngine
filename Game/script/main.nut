function main(){
    print("test starting...\n");
    wait(10);
    local e = Enemy(testing, 320.0, 100.0, 70.0, 5);
    wait(30);
    e.Kill();
    e = null;
    wait(60);
    for (local i = 0; i < 10; i++) {
        Enemy(testing, 100.0, 300.0, 70.0, 5);
    }
    //print("e: " + e + "\n");
    //e.Kill();
    wait(200);
    local l = ____enm.len();
    for (local i = 0; i < l; i++) {
        print("idx[" + i + "]: " + ____enm[i] + "\n");
    }
    wait(2000);
}

tex <- Texture("graph/dialogue_morgan.tga");

function testing(a, b, c, d) {
    texture = tex;
    local x = a;
    local y = b;
    print("a is: " + a + " b is: " + b + " c is: " + c + " d is: " + d + "!\n");
    SetPos(vec2t(x, y));
    SetScale(vec2t(1.0, 1.0));
    SetSize(vec2t(256.0, 256.0));
    SetUV(vec4t(0.0, 0.0, 0.7, 0.7));
    for (local i = 0; i < 100; i++){
        SetPos(vec2t(a, y + sin(i * (3.14159 / 180.0) * 2.0) * c));
        wait(1);
    }
    Kill()
    print("goodbye!");
}