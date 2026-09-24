#include "dialogue.h"

namespace sg
{

int Dialogue::_find(int id)
{
    for(int i = 0; i < _choices.size(); ++i)
    {
        if(_choices[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

void Dialogue::add_dialogue(int id, const bn::string_view& query, const bn::string_view& reply,
                             DialogueChoiceType type, bool hidden)
{
    if(_find(id) >= 0 || _choices.full())
    {
        return;
    }

    _choices.push_back({ id, bn::string<40>(query), bn::string<200>(reply), type, hidden });
}

void Dialogue::add_quit(int id, const bn::string_view& query, const bn::string_view& reply, bool hidden)
{
    add_dialogue(id, query, reply, DialogueChoiceType::QUIT, hidden);
}

void Dialogue::show_dialogue(int id)
{
    int index = _find(id);

    if(index >= 0)
    {
        _choices[index].hidden = false;
    }
}

void Dialogue::hide_dialogue(int id)
{
    int index = _find(id);

    if(index >= 0)
    {
        _choices[index].hidden = true;
    }
}

void Dialogue::talk(RoomScreen& screen, const bn::string_view& opening_line,
                     const bn::sprite_item* portrait)
{
    _choices.clear();
    initialize_choices();

    if(portrait)
    {
        screen.show_overlay_sprite(*portrait);
    }

    screen.show_text(opening_line);

    while(true)
    {
        bn::vector<bn::string<40>, max_choices + 1> labels;
        bn::vector<int, max_choices + 1> ids;

        for(const Entry& entry : _choices)
        {
            if(! entry.hidden)
            {
                labels.push_back(entry.query);
                ids.push_back(entry.id);
            }
        }

        labels.push_back(bn::string<40>("Leave"));

        if(labels.size() == 1)
        {
            break;
        }

        int picked = screen.choose(labels);

        if(picked == labels.size() - 1)
        {
            break;
        }

        int id = ids[picked];
        int index = _find(id);
        Entry entry = _choices[index];
        screen.show_text(entry.reply);

        bool quit = entry.type == DialogueChoiceType::QUIT;

        if(quit || entry.type == DialogueChoiceType::ONCE)
        {
            _choices.erase(_choices.begin() + index);
        }

        on_choice_selected(id, screen);

        if(quit)
        {
            break;
        }
    }

    if(portrait)
    {
        screen.hide_overlay_sprite();
    }
}

}
