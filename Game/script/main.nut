function main(){
    SetBulletTexture("graph/ball.tga");
    print("test starting...\n");
    while (1) {
        wait(10);
        Enemy(testing, 320.0, 300.0, 70.0, tex);
        //Enemy(testing, 400.0, 300.0, 70.0, tex2);
        //print("e: " + e + "\n");
        //e.Kill();
        wait(400);
    }
    wait(2000);
}

tex <- Texture("graph/dialogue_morgan.tga");
tex2 <- Texture("graph/yuuta.tga");
ball <- Texture("graph/ball.tga");

function testing(x, y, c, tex) {
    texture = tex;
    //print("a is: " + a + " b is: " + b + " c is: " + c + " d is: " + d + "!\n");
    SetPos(vec2t(x, y));
    SetRadius(30.0);
    SetScale(vec2t(1.0, 1.0));
    SetSize(vec2t(256.0, 256.0));
    SetUV(vec4t(0.0, 0.0, 0.7, 0.7));
    local ny;
    //SetUV(vec4t(0.0, 0.0, 1.0, 1.0));
    for (local i = 0; i < 300; i++){
        ny = y + sin(i * (3.14159 / 180.0) * 2.0) * c;
        SetPos(vec2t(x, ny));
        for (local j = 0; j < 1; j++) {
            //Bullet(bullet_test, x, ny, (rand() % 360).tofloat() * 3.1415926 / 180.0);
        }
        wait(1);
    }
    //print("goodbye!");
}

function bullet_test(x, y, dir) {
    SetPos(vec2t(x, y));
    SetRadius(10.0);
    SetSize(vec2t(24.0));
    local vx = cos(dir) * 2.0;
    local vy = sin(dir) * 2.0;
    local nx = x;
    local ny = y;
    for (local i = 0; i < 300; i++) {
        nx += vx;
        ny += vy;
        SetPos(vec2t(nx, ny));
        SetRotation(dir + i * 0.1);
        wait(1);
    }
}