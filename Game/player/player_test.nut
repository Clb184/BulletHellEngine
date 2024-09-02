MORGAN_UNIT <- 1.0 / 1024.0;
morgan_idle <- vec4t(0.0 * MORGAN_UNIT, 0.0, (160.0) * MORGAN_UNIT, (180.0) * MORGAN_UNIT);
morgan_tex <- Texture("graph/morgan_pl.tga");

LoadSound(0, "sound/se_shot.ogg", 3);

function main() {
    local start = vec2t(0.0, 500.0);
    local y = 500.0;
    player.texture = morgan_tex;
    player.SetMutekiTime(200);
    player.SetColor(0xffffffff);
    player.SetSize(vec2t(56.88, 64.0));
    player.SetRadius(2.0);
    player.SetUV(morgan_idle);
    player.EnableMove(false);
    for (local i = 0; i < 60; i++) {
        player.SetPos(start);
        wait(1);
        start.y(y);
        y -=  1.5;
    }
    player.EnableMove(true);
    while (1){
        PlaySound(0, player.x);
        PlayerShot(shot_test, player.x - 10.0, player.y, PI * -0.5, 22.0);
        PlayerShot(shot_test, player.x + 10.0, player.y, PI * -0.5, 22.0);
        //PlayerShot(shot_test, player.x, player.y, PI * -0.5 - 0.4, 12.0);
        //PlayerShot(shot_test, player.x, player.y, PI * -0.5 - 0.2, 12.0);
        //PlayerShot(shot_test, player.x, player.y, PI * -0.5, 12.0);
        //PlayerShot(shot_test, player.x, player.y, PI * -0.5 + 0.2, 12.0);
        //PlayerShot(shot_test, player.x, player.y, PI * -0.5 + 0.4, 12.0);
        wait(5);
    }
}

function OnDeath() {
    player.SetColor(0xff0000ff);
    wait(60);
}

function shot_test(x, y, dir, speed) {
    local pos = vec2t(x, y);
    local vx = cos(dir) * speed;
    local vy = sin(dir) * speed;
    SetRadius(10.0);
    SetDamage(5);
    SetPos(pos);
    while (pos.y() > 0) {
        SetPos(pos);
        wait(1);
        pos.y(pos.y() + vy);
        pos.x(pos.x() + vx);
    }
}