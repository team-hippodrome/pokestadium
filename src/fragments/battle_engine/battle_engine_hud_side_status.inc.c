void BattleHud_DrawOwnSideStatus(ActivePokemon* active_pokemon, ActivePokemonState* state) {
    TeamRoster* roster = active_pokemon->sessionTeams->tagTeamMember[active_pokemon->thumbnail.tagTeam];
    UNUSED s32 pad;
    s32 higher_port_member;

    if (gSessionContext.mode != SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        if (state->selectedCommandOption == 3) {
            BattleHud_DrawMonStatusBoxPlayer(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 2,
                                             active_pokemon->thumbnail.camera->viewport.y - 0x3A,
                                             state->actionDelayTimer, 1);
        } else {
            BattleHud_DrawMonStatusBoxPlayer(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 2,
                                             active_pokemon->thumbnail.camera->viewport.y - 0x3A,
                                             state->actionDelayTimer, 0);
        }

        if ((state->selectedCommandOption != 3) && (active_pokemon->thumbnail.camera->viewport.x == 0x17)) {
            switch (state->selectedCommandOption) {
                case 2:
                    if (state->showingCommandOptions == 1) {
                        if (active_pokemon->sessionTeams->tagTeamSize == 2) {
                            if (active_pokemon->sessionTeams->tagTeamMember[1]->controllerPort <
                                active_pokemon->sessionTeams->tagTeamMember[0]->controllerPort) {
                                higher_port_member = 0;
                            } else {
                                higher_port_member = 1;
                            }
                            BattleHud_DrawTeamStatusList(
                                active_pokemon, active_pokemon->sessionTeams->tagTeamMember[higher_port_member], 96, 0xF,
                                active_pokemon->sessionTeams->tagTeamMember[higher_port_member]->controllerPort);
                            BattleHud_DrawTeamStatusList(
                                active_pokemon, active_pokemon->sessionTeams->tagTeamMember[higher_port_member == 0], 198,
                                0xF, active_pokemon->sessionTeams->tagTeamMember[higher_port_member == 0]->controllerPort);
                        } else if (active_pokemon->teamRoster->numberOfPokemonBrought < 4) {
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->tagTeamMember[0],
                                                         96, 0xF, roster->controllerPort);
                        } else {
                            BattleHud_DrawDoublesTeamStatusList(active_pokemon, 96, 0xF, roster->controllerPort);
                        }
                    } else if (state->replacementState == 0x10) {
                        BattleHud_DrawOneOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    }
                    break;

                case 1:
                    if (state->showingCommandOptions == 1) {
                        BattleHud_DrawMoveSelectMenu(active_pokemon, 96, 0xF, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    }
                    break;

                case 4:
                    BattleHud_DrawTwoOptionCursorMenu(active_pokemon, 0x5F, 14, roster->controllerPort);
                    break;

                default:
                    BattleHud_DrawThreeOptionLabelBar(active_pokemon, 96, 0xF, roster->controllerPort);
                    break;
            }
        }
    }
}

void BattleHud_DrawOpponentSideStatus(ActivePokemon* active_pokemon, ActivePokemonState* state) {
    TeamRoster* roster = active_pokemon->sessionTeams->tagTeamMember[active_pokemon->thumbnail.tagTeam];
    s32 higher_port_member;

    if (gSessionContext.mode != SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        if (state->selectedCommandOption == 3) {
            BattleHud_DrawMonStatusBoxOpponent(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 0x20,
                                               active_pokemon->thumbnail.camera->viewport.y - 2,
                                               state->actionDelayTimer, 1);
        } else {
            BattleHud_DrawMonStatusBoxOpponent(active_pokemon, active_pokemon->thumbnail.camera->viewport.x - 0x20,
                                               active_pokemon->thumbnail.camera->viewport.y - 2,
                                               state->actionDelayTimer, 0);
        }

        if ((state->selectedCommandOption != 3) && (active_pokemon->thumbnail.camera->viewport.x == 0x103)) {
            switch (state->selectedCommandOption) {
                case 2:
                    if (state->showingCommandOptions == 1) {
                        if (active_pokemon->sessionTeams->tagTeamSize == 2) {
                            if (active_pokemon->sessionTeams->tagTeamMember[1]->controllerPort <
                                active_pokemon->sessionTeams->tagTeamMember[0]->controllerPort) {
                                higher_port_member = 0;
                            } else {
                                higher_port_member = 1;
                            }
                            BattleHud_DrawTeamStatusList(
                                active_pokemon, active_pokemon->sessionTeams->tagTeamMember[higher_port_member == 0], 0x16,
                                0x79,
                                active_pokemon->sessionTeams->tagTeamMember[higher_port_member == 0]->controllerPort);
                            BattleHud_DrawTeamStatusList(
                                active_pokemon, active_pokemon->sessionTeams->tagTeamMember[higher_port_member], 0x7C,
                                0x79, active_pokemon->sessionTeams->tagTeamMember[higher_port_member]->controllerPort);
                        } else if (active_pokemon->teamRoster->numberOfPokemonBrought < 4) {
                            BattleHud_DrawTeamStatusList(active_pokemon, active_pokemon->sessionTeams->tagTeamMember[0],
                                                         0x7C, 0x79, roster->controllerPort);
                        } else {
                            BattleHud_DrawDoublesTeamStatusList(active_pokemon, 0x18, 0x79, roster->controllerPort);
                        }
                    } else if (state->replacementState == 0x10) {
                        BattleHud_DrawOneOptionLabelBar(active_pokemon, 0x98, 0xCE, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 0x62, 0xCE, roster->controllerPort);
                    }
                    break;

                case 1:
                    if (state->showingCommandOptions == 1) {
                        BattleHud_DrawMoveSelectMenu(active_pokemon, 0x19, 0xA6, roster->controllerPort);
                    } else {
                        BattleHud_DrawTwoOptionLabelBar(active_pokemon, 0x62, 0xCE, roster->controllerPort);
                    }
                    break;

                case 4:
                    BattleHud_DrawTwoOptionCursorMenu(active_pokemon, 0x17, 0xBB, roster->controllerPort);
                    break;

                default:
                    BattleHud_DrawThreeOptionLabelBar(active_pokemon, 0x18, 0xCE, roster->controllerPort);
                    break;
            }
        }
    }
}
