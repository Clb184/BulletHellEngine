VIVIT_TEX_UNIT <- 1.0 / 1024.0

viv_idle <- [
    vec4t(0.0 * VIVIT_TEX_UNIT, 0.0 * VIVIT_TEX_UNIT, 80.0 * VIVIT_TEX_UNIT, 100.0 * VIVIT_TEX_UNIT),
    vec4t(80.0 * VIVIT_TEX_UNIT, 0.0 * VIVIT_TEX_UNIT, 160.0 * VIVIT_TEX_UNIT, 100.0 * VIVIT_TEX_UNIT),
    vec4t(160.0 * VIVIT_TEX_UNIT, 0.0 * VIVIT_TEX_UNIT, 240.0 * VIVIT_TEX_UNIT, 100.0 * VIVIT_TEX_UNIT),
    vec4t(240.0 * VIVIT_TEX_UNIT, 0.0 * VIVIT_TEX_UNIT, 320.0 * VIVIT_TEX_UNIT, 100.0 * VIVIT_TEX_UNIT),
]

sumi_spr <- vec4t(0.0, 0.0, 96.0, 96.0) / 512.0;

viv_tex <- Texture("graph/vivit.tga");

function main(){
    SetBulletTexture("graph/ball.tga");
    //print("test starting...\n");
    wait(60);
    while (1) {
    Enemy(testing, 0.0, 150.0, 20.0, sumi);
        wait(700);
    }
    while (1) {
        wait(10);
        local x = -150.0;
        local y = 100.0;
        for (local i = 0; i < 10; i++){
            Enemy(testing, x, y, 20.0, viv_tex);
            x += 30.0;
            y += 10.0;
            wait(1);
        }
        wait(600);
        x -= 30.0;
        for (local i = 0; i < 10; i++){
            Enemy(testing, x, y, 20.0, viv_tex);
            x -= 30.0;
            y += 10.0;
            wait(1);
        }
        //Enemy(testing, 400.0, 300.0, 70.0, tex2);
        //print("e: " + e + "\n");
        //e.Kill();
        wait(700);
    }
    wait(2000);
}

tex <- Texture("graph/dialogue_morgan.tga");
tex2 <- Texture("graph/yuuta.tga");
ball <- Texture("graph/ball.tga");
sumi <- Texture("graph/sumi.tga");

function testing(x, y, c, tex) {
    local ny;
    texture = tex;
    //print("a is: " + a + " b is: " + b + " c is: " + c + " d is: " + d + "!\n");
    SetPos(vec2t(x, y));
    SetRadius(16.0);
    SetScale(vec2t(1.0, 1.0));
    SetSize(vec2t(80.0, 100.0));
    SetSize(vec2t(96.0, 96.0));
    SetUV(viv_idle[0]);
    SetUV(sumi_spr);
    SetLife(2000);
    //SetUV(vec4t(0.0, 0.0, 1.0, 1.0));
    for (local i = 0; i < 600; i++){
        ny = y + sin(i * (3.14159 / 180.0) * 2.0) * c;
        SetPos(vec2t(x, ny));
        if (i == 300){
            Bullet(bullet_test, x, ny, PI * 0.5, 5.0);
            for (local j = 0; j < 16; j++) {
                Bullet(bullet_test, x, ny, (rand() % 360).tofloat() * 3.1415926 / 180.0, 1.0);
                //print("j: " + j + "\n");
            }
        }
        wait(1);
    }
    //print("bye\n");
    //print("goodbye!");
}

function bullet_test(x, y, dir,  speed) {
    SetPos(vec2t(x, y));
    SetRadius(10.0);
    SetSize(vec2t(24.0));
    local vx = cos(dir) * speed;
    local vy = sin(dir) * speed;
    local nx = x;
    local ny = y;
    for (local i = 0; i < 400; i++) {
        nx += vx;
        ny += vy;
        SetPos(vec2t(nx, ny));
        SetRotation(dir + i * 0.1);
        wait(1);
    }
}