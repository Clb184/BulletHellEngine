MORGAN_UNIT <- 1.0 / 1024.0;
morgan_idle <- vec4t(0.0 * MORGAN_UNIT, 0.0, (160.0) * MORGAN_UNIT, (180.0) * MORGAN_UNIT);
morgan_tex <- Texture("graph/morgan_pl.tga");

function main() {
    local start = vec2t(320.0, 500.0);
    local y = 500.0;
    player.texture = morgan_tex;
    player.SetColor(0xffffffff);
    player.SetSize(vec2t(56.88, 64.0));
    player.SetRadius(24.0);
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
        wait(1);
    }
}

function OnDeath() {
    player.SetColor(0xff0000ff);
    wait(200);
}