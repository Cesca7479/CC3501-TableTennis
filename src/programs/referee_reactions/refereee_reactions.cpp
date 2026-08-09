#include <stdio.h>
#include "pico/stdlib.h"

#include "referee_reactions.h"
#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"
#include "helpers/colours/colours.h"
#include "drivers/logging/logging.h"
#include "drivers/piezos/piezos.h"
#include "drivers/buzzer/buzzer.h"

#include "helpers/timing/timing.h"

#include "game/gamestate.h"

// Preset notes in each octave
static constexpr uint32_t notes6[7] = {1047, 1175, 1319, 1397, 1568, 1760, 1976};
static constexpr uint32_t notes7[7] = {2093, 2349, 2637, 2794, 3136, 3520, 3951};


// Dance sequence configuration
// Preset led colours and buzzer notes
struct DanceStep
{
    uint32_t frequency;
    LedColour colour;
};

static constexpr DanceStep DANCE_TUNE[] =
    {
        {notes6[2], PURPLE},
        {notes6[0], PINK},
        {notes6[2], PURPLE},
        {notes6[4], BLUE},
        {notes6[2], PURPLE},
        {notes6[4], BLUE},
        {notes7[0], PINK},
        {notes7[0], PINK},
        {notes7[0], PINK}};

// adjustable parameters for dance sequence
static constexpr uint32_t DANCE_STEP_INTERVAL_MS = 100;
static constexpr uint32_t DANCE_UPDATE_INTERVAL_MS = 10;
static constexpr uint8_t DANCE_REPEAT_COUNT = 2;

static constexpr size_t DANCE_TUNE_LENGTH = sizeof(DANCE_TUNE) / sizeof(DANCE_TUNE[0]);

struct DanceState
{
    uint8_t step_index = 0;
};

struct DanceSection
{
    uint32_t duration_ms;
    uint32_t move_interval_ms;
    uint32_t pause_after_ms;
};

static constexpr DanceSection VICTORY_DANCE[] =
    {
        {1000, 500, 0},
        {500, 250, 250}};


// Helper to stop motor, leds and buzzer once
static void stop_all()
{
    motor_disable();
    clear_all_leds();
    buzzer_stop();
}

// Helper to determine which the ServoPosition given which player is passed in
static ServoPosition convert_player_to_side(uint8_t player)
{
    return (player == PLAYER_1) ? LEFT : RIGHT;
}

/**
 * @brief Light led on side of specified player
 * @param player_side The side the specified player is on
 * @param colour The colour to light the led
 */
static void light_player_side(ServoPosition player_side, rgb_colour colour)
{
    const uint8_t led =(player_side == LEFT) ? 1 : 0;
    set_single_led(led, colour);
    update_all_leds();
}

/**
 * @brief Runs one section of the referee dance
 *
 * Motor, LEDs and buzzer are timed independently
 *
 * @param duration_ms Total duration of this dance section
 * @param move_interval_ms Time between arm position changes
 * @param state Persistent dance state so colour/tune continues between sections
 *
 * @return true if completed normally, false if interrupted or motor fault occurred
 */
static bool run_dance_section(const DanceSection &section, DanceState &state)
{
    const absolute_time_t dance_end = make_timeout_time_ms(section.duration_ms);
    ServoPosition position = LEFT;

    // Start first movement immediately
    motor_set_position(position);
    if (!motor_enable())
    {
        return false;
    }

    absolute_time_t next_move_time = make_timeout_time_ms(section.move_interval_ms);
    absolute_time_t next_note_time = get_absolute_time();

    // Dance sequence
    while (!time_reached(dance_end))
    {
        // Motor fault checking, on fault stop animation
        if (is_motor_fault_active())
        {
            log(LogLevel::ERROR, "Motor fault, over-current or high temperature detected. Motor power disabled.");
            stop_all();
            return false;
        }

        // Motor
        if (time_reached(next_move_time))
        {
            position = (position == LEFT) ? RIGHT : LEFT;
            motor_set_position(position);
            next_move_time = make_timeout_time_ms(section.move_interval_ms);
        }

        // Buzzer + LEDs
        if (time_reached(next_note_time))
        {
            const DanceStep &note = DANCE_TUNE[state.step_index];
            // Play note
            buzzer_play_tone(note.frequency);
            // Display colour associated with note
            set_all_leds(get_rgb(note.colour));
            update_all_leds();
            // Advance both together
            state.step_index = (state.step_index + 1) % DANCE_TUNE_LENGTH;
            next_note_time = make_timeout_time_ms(DANCE_STEP_INTERVAL_MS);
        }

        // Allow button interruption
        if (sleep_ms_with_button_checking(DANCE_UPDATE_INTERVAL_MS))
        {
            stop_all();
            return false;
        }
    }

    stop_all();
    return true;
}

void referee_dance_sequence(uint8_t winner)
{
    const ServoPosition winner_side = convert_player_to_side(winner);
    DanceState dance_state;

    // Full dance sequence is the preset VICTORY_DANCE played twice, where DANCE_TUNE restarts between
    for (uint8_t repeat = 0; repeat < DANCE_REPEAT_COUNT; repeat++)
    {
        // Restart tune/LED sequence at beginning of each repetition
        dance_state.step_index = 0;
        for (const DanceSection &section : VICTORY_DANCE)
        {
            if (!run_dance_section(section, dance_state))
            {
                return;
            }

            if (section.pause_after_ms > 0)
            {
                if (sleep_ms_with_button_checking(section.pause_after_ms))
                {
                    return;
                }
            }
        }
    }

    // Final sequence, with only winner side raised and lit
    clear_all_leds();
    motor_move_motor_safely(winner_side);
    light_player_side(winner_side, get_rgb(YELLOW));
    buzzer_play_victory_sequence();
    motor_move_motor_safely(CENTRE);
    clear_all_leds();
}

void referee_indicate_server(uint8_t player)
{
    ServoPosition side_serving = convert_player_to_side(player);
    buzzer_play_tone(notes6[0]);
    if (sleep_ms_with_button_checking(200))
        return;
    buzzer_play_tone(notes6[2]);
    motor_move_motor_safely(side_serving);
    light_player_side(side_serving, get_rgb(WHITE));
    buzzer_play_tone(notes6[4]);
    if (sleep_ms_with_button_checking(200))
        return;
    buzzer_stop();
    if (sleep_ms_with_button_checking(500))
        return;
    motor_move_motor_safely(CENTRE);
}

void referee_point_scored(uint8_t player)
{
    ServoPosition side_scored = convert_player_to_side(player);
    buzzer_play_tone(notes6[6]);
    motor_move_motor_safely(side_scored);
    light_player_side(side_scored, get_rgb(GREEN));
    buzzer_play_tone(notes7[2]);
    if (sleep_ms_with_button_checking(200))
        return;
    buzzer_stop();
    clear_all_leds();
}

void referee_angry(uint duration_ms)
{
    set_all_leds(get_rgb(RED));
    update_all_leds();
    buzzer_play_angry_sounds();
    absolute_time_t end_time = make_timeout_time_ms(duration_ms);
    while (!time_reached(end_time))
    {
        motor_move_motor_safely(LEFT);
        set_all_leds(get_rgb(RED));
        update_all_leds();
        motor_disable();

        motor_move_motor_safely(RIGHT);
        set_all_leds(get_rgb(OFF));
        update_all_leds();
        for (uint8_t i = 0; i < 5; i++)
        {
            buzzer_play_tone(notes7[0]);
            if (sleep_ms_with_button_checking(20))
                return;
            buzzer_play_tone(notes6[6]);
            if (sleep_ms_with_button_checking(20))
                return;
        }
    }
    motor_move_motor_safely(CENTRE);
    buzzer_stop();
}